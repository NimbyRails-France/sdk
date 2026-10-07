#!/bin/sh
# Run in the CI build container (node:24-bookworm), never on the production host.
set -eu
commit=5948781ee5889e84ecd1b530ca6284ebc2953b28
checkout="$PWD/.ci/wiki-$commit"
test ! -e "$checkout"
command -v git >/dev/null
node -e 'if (Number(process.versions.node.split(".")[0]) !== 24) process.exit(1)'
mkdir -p .ci
git clone --no-checkout https://github.com/NimbyRails-France/wiki.git "$checkout"
git -C "$checkout" checkout --detach "$commit"
test "$(git -C "$checkout" rev-parse HEAD)" = "$commit"
cd "$checkout"
npm ci --no-audit --no-fund
npm test
npm run typecheck
npm run generate
npm run check:generated
test -s .output/public/version/0.9/index.html
test -s .output/public/en/version/0.9/index.html
test -s .output/public/version/0.8/index.html
test -s .output/public/en/version/0.8/index.html
printf 'Verified static wiki from commit %s at %s/.output/public\n' "$commit" "$checkout"
