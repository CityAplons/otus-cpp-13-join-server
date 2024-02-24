#pragma once

struct ProjectInfo {
  int major, minor, revision;
  const char *nameString;
  const char *versionString;

  ProjectInfo(void);
};
