# Read-only research tool. These fields are candidates, not a persistence API.
param([Parameter(Mandatory=$true)][string[]]$Path)
$ErrorActionPreference = 'Stop'

function Read-Header([IO.FileStream]$Stream) {
    $Stream.Position = 0
    $buffer = New-Object byte[] 1380
    $offset = 0
    while ($offset -lt $buffer.Length) {
        $count = $Stream.Read($buffer,$offset,$buffer.Length-$offset)
        if ($count -eq 0) { throw 'Truncated save header' }
        $offset += $count
    }
    return ,$buffer
}

foreach ($savePath in $Path) {
    $file = Get-Item -LiteralPath $savePath
    $stream = [IO.File]::Open($file.FullName,[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::ReadWrite)
    try {
        $first = Read-Header $stream
        $second = Read-Header $stream
        if ([Convert]::ToBase64String($first) -ne [Convert]::ToBase64String($second)) {
            throw 'Save header changed during observation'
        }
        if ([BitConverter]::ToUInt32($first,0) -ne 0x59424d4e -or
            [BitConverter]::ToUInt16($first,4) -ne 2 -or
            [BitConverter]::ToUInt16($first,6) -ne 1) {
            throw 'Unsupported save header variant'
        }
        # Writer RVA 0x49f050 copies Game+0xb58 and the final history entry to
        # header offsets 0x4d8 and 0x4f8 respectively. No payload is decoded.
        [pscustomobject]@{
            File=$file.Name
            Format=[BitConverter]::ToUInt32($first,16)
            VersioningValue=[BitConverter]::ToString($first,0x4d8,32).Replace('-','').ToLowerInvariant()
            LastHistoryValue=[BitConverter]::ToString($first,0x4f8,32).Replace('-','').ToLowerInvariant()
        }
    } finally { $stream.Dispose() }
}
