#!/bin/bash
set -x
set -e
set -u

cd "${CLONE_DIR}"

for f in "${INSTALL_DIR}/bin/"*; do
  echo "${COMMIT_ID}">"${f}.version"
done

cd "${INSTALL_DIR}"
zip -r "../${INSTALL_DIR}.zip" *
cd ..

sha1sum "${INSTALL_DIR}.zip" >"${INSTALL_DIR}.zip.sha1"

sed -e "s/@GROUP@/${GROUP_DOTS}/g" -e "s/@ARTIFACT@/${ARTIFACT}/g" -e "s/@VERSION@/${VERSION}/g" "fake_pom.xml" >"${POM_FILE}"
sha1sum "${POM_FILE}" >"${POM_FILE}.sha1"

DESCRIPTION="$(echo -e "Automated build.\n$(git log --graph -n 3 --abbrev-commit --pretty='format:%h - %s <%an>')")"

github-release \
  "${GITHUB_USER}/${GITHUB_REPO}" \
  "${TAG}" \
  "${COMMIT_ID}" \
  "${DESCRIPTION}" \
  "${INSTALL_DIR}.zip"

github-release \
  "${GITHUB_USER}/${GITHUB_REPO}" \
  "${TAG}" \
  "${COMMIT_ID}" \
  "${DESCRIPTION}" \
  "${INSTALL_DIR}.zip.sha1"

# Don't fail if pom cannot be uploaded, as it might already be there.

github-release \
  "${GITHUB_USER}/${GITHUB_REPO}" \
  "${TAG}" \
  "${COMMIT_ID}" \
  "${DESCRIPTION}" \
  "${POM_FILE}" || true

github-release \
  "${GITHUB_USER}/${GITHUB_REPO}" \
  "${TAG}" \
  "${COMMIT_ID}" \
  "${DESCRIPTION}" \
  "${POM_FILE}.sha1" || true
