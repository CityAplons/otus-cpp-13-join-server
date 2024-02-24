#include <gtest/gtest.h>

#include "project.h"

TEST(TemplateVersionTest, BasicAssertions) {
  struct ProjectInfo info = {};
  EXPECT_GT(info.revision, 0);
}