#!/bin/bash

# 删除本地标签
git tag -d v1.0.0

# 删除远程标签
git push origin :refs/tags/v1.0.0

# 重新创建标签
git tag v1.0.0

# 推送新标签（这次会成功创建 Release！）
git push origin v1.0.0