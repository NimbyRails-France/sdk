14031bea0 MOV qword ptr [RSP + 0x10],RBX
14031bea5 MOV qword ptr [RSP + 0x18],RSI
14031beaa PUSH RDI
14031beab SUB RSP,0x30
14031beaf MOV RDI,RCX
14031beb2 XOR ESI,ESI
14031beb4 MOV ECX,0xe8
14031beb9 CALL 0x140983da8
14031bebe XOR EDX,EDX
14031bec0 MOV qword ptr [RSP + 0x40],RAX
14031bec5 MOV R8D,0xe8
14031becb MOV RCX,RAX
14031bece MOV RBX,RAX
14031bed1 CALL 0x140985048
14031bed6 MOV qword ptr [RBX + 0x8],RSI
14031beda LEA RAX,[0x140a6dde8]
14031bee1 MOV qword ptr [RBX],RAX
14031bee4 XORPS XMM0,XMM0
14031bee7 MOV qword ptr [RBX + 0x10],RSI
14031beeb MOV RAX,RDI
14031beee MOV qword ptr [RBX + 0x18],RSI
14031bef2 MOV qword ptr [RBX + 0x20],RSI
14031bef6 MOV byte ptr [RBX + 0x28],0x1
14031befa MOVUPS xmmword ptr [RBX + 0x30],XMM0
14031befe MOV qword ptr [RBX + 0x40],RSI
14031bf02 MOV qword ptr [RBX + 0x48],0xf
14031bf0a MOV byte ptr [RBX + 0x30],SIL
14031bf0e MOV dword ptr [RBX + 0x50],ESI
14031bf11 MOV qword ptr [RBX + 0x58],RSI
14031bf15 MOV qword ptr [RBX + 0x60],RSI
14031bf19 MOV qword ptr [RBX + 0x68],RSI
14031bf1d MOV qword ptr [RBX + 0x78],RSI
14031bf21 MOV dword ptr [RBX + 0x80],ESI
14031bf27 MOV byte ptr [RBX + 0x84],SIL
14031bf2e MOV dword ptr [RBX + 0x90],ESI
14031bf34 MOV qword ptr [RBX + 0x98],RSI
14031bf3b MOV qword ptr [RBX + 0xa0],RSI
14031bf42 MOV qword ptr [RBX + 0xa8],RSI
14031bf49 MOV qword ptr [RBX + 0xb0],RSI
14031bf50 MOV qword ptr [RBX + 0xb8],RSI
14031bf57 MOV qword ptr [RBX + 0xc0],RSI
14031bf5e MOV RSI,qword ptr [RSP + 0x50]
14031bf63 MOV word ptr [RBX + 0x70],0x101
14031bf69 MOV dword ptr [RBX + 0x88],0x708
14031bf73 MOV word ptr [RBX + 0x8c],0x101
14031bf7c MOV qword ptr [RDI],RBX
14031bf7f MOV RBX,qword ptr [RSP + 0x48]
14031bf84 ADD RSP,0x30
14031bf88 POP RDI
14031bf89 RET
