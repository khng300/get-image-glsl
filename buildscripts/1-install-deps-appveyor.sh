#!/bin/bash
set -x
set -e
set -u

GITHUB_RELEASE_TOOL_USER="c4milo"
GITHUB_RELEASE_TOOL_VERSION="v1.1.0"
GITHUB_RELEASE_TOOL_ARCH="windows_amd64"

mkdir temp
cd temp

curl -fsSL -o github-release.tar.gz "https://github.com/${GITHUB_RELEASE_TOOL_USER}/github-release/releases/download/${GITHUB_RELEASE_TOOL_VERSION}/github-release_${GITHUB_RELEASE_TOOL_VERSION}_${GITHUB_RELEASE_TOOL_ARCH}.tar.gz"

7z x github-release.tar.gz
7z x github-release.tar
cd ..

mkdir -p deps
cd deps
curl -fsSL -o angle.zip https://github.com/paulthomson/build-angle/releases/download/v-592879ad24e66c7c68c3a06d4e2227630520da36/MSVC2015-Release-x64.zip
7z x angle.zip
