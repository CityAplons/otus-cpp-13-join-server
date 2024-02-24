#include "project.h"
#include "version.h"

ProjectInfo::ProjectInfo() {
  this->major = PROJECT_VERSION_MAJOR;
  this->minor = PROJECT_VERSION_MINOR;
  this->revision = PROJECT_VERSION_REVISION;

  this->versionString = PROJECT_VERSION_STRING;
  this->nameString = PROJECT_NAME;
}
