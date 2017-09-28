#!/bin/bash
set -x
set -e
set -u

sudo mkdir -p /data/bin
sudo chmod uga+rwx /data/bin

GITHUB_RELEASE_TOOL_USER="paulthomson"
GITHUB_RELEASE_TOOL_VERSION="v1.0.9.1"

if [ "$(uname)" == "Darwin" ];
then
  GITHUB_RELEASE_TOOL_ARCH="darwin_amd64"
fi

if [ "$(uname)" == "Linux" ];
then
  GITHUB_RELEASE_TOOL_ARCH="linux_amd64"
  sudo apt-get -y install g++ libxrandr-dev libxinerama-dev libxcursor-dev libgl1-mesa-dev cmake zip git

  # The JSON library requires GCC >= 4.9
  sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y;
  sudo apt-get update -qq
  sudo apt-get install -qq -y g++-4.9
  sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 90

fi

pushd /data/bin
wget "https://github.com/${GITHUB_RELEASE_TOOL_USER}/github-release/releases/download/${GITHUB_RELEASE_TOOL_VERSION}/github-release_${GITHUB_RELEASE_TOOL_VERSION}_${GITHUB_RELEASE_TOOL_ARCH}.tar.gz"
tar xf "github-release_${GITHUB_RELEASE_TOOL_VERSION}_${GITHUB_RELEASE_TOOL_ARCH}.tar.gz"
popd
