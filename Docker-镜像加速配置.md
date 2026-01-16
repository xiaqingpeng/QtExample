# 🚀 Docker 镜像加速配置

## 问题

构建 Docker 镜像时遇到：
```
failed to solve: ubuntu:22.04: failed to resolve source metadata
net/http: TLS handshake timeout
```

这是 Docker Hub 连接超时的问题。

## 解决方案

### 方案 1：配置 Docker Desktop 镜像加速器（推荐）

#### macOS Docker Desktop

1. 打开 **Docker Desktop**
2. 点击右上角 **设置图标**（齿轮）
3. 选择 **Docker Engine**
4. 在 JSON 配置中添加：

```json
{
  "registry-mirrors": [
    "https://docker.mirrors.ustc.edu.cn",
    "https://registry.docker-cn.com",
    "https://mirror.ccs.tencentyun.com"
  ]
}
```

5. 点击 **Apply & Restart**

#### 国内常用镜像源

- **中科大镜像**：`https://docker.mirrors.ustc.edu.cn`
- **Docker 中国**：`https://registry.docker-cn.com`
- **腾讯云**：`https://mirror.ccs.tencentyun.com`
- **阿里云**：需要登录阿里云获取专属加速地址

### 方案 2：使用命令行配置（Linux）

编辑或创建 `/etc/docker/daemon.json`：

```json
{
  "registry-mirrors": [
    "https://docker.mirrors.ustc.edu.cn",
    "https://registry.docker-cn.com"
  ]
}
```

然后重启 Docker：
```bash
sudo systemctl restart docker
```

### 方案 3：验证配置

配置完成后，验证是否生效：

```bash
docker info | grep -A 10 "Registry Mirrors"
```

应该能看到配置的镜像源。

### 方案 4：临时使用代理（如果镜像源不可用）

如果镜像源也不可用，可以配置代理：

```bash
# 设置代理环境变量
export HTTP_PROXY=http://your-proxy:port
export HTTPS_PROXY=http://your-proxy:port

# 然后构建
docker-compose build --no-cache qt-dev
```

## 重新构建

配置完成后，重新构建：

```bash
docker-compose build --no-cache qt-dev
```

## 故障排除

### 如果仍然超时

1. **检查网络连接**：
   ```bash
   ping registry-1.docker.io
   ```

2. **尝试使用其他基础镜像**：
   如果 Ubuntu 镜像拉取困难，可以尝试使用其他镜像源

3. **使用已有镜像**：
   如果本地已有 Ubuntu 镜像，可以直接使用：
   ```bash
   docker images | grep ubuntu
   ```

4. **稍后重试**：
   网络问题可能是临时的，稍后重试可能成功

## 验证

配置完成后，测试拉取镜像：

```bash
docker pull ubuntu:22.04
```

如果成功，说明配置生效。

---

**配置完成后，重新运行构建命令即可！** 🎉

