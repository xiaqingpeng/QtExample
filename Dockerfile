# Qt Linux 桌面开发环境
# 基于 Ubuntu 22.04，包含 Qt6 开发环境

FROM ubuntu:22.04

# 避免交互式提示
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Shanghai

# 可选：切换 apt 镜像源以规避 ports.ubuntu.com 502/超时问题（特别是 ARM64）
# - 默认使用官方源
# - 构建时可传：--build-arg UBUNTU_MIRROR=https://mirrors.tuna.tsinghua.edu.cn
ARG UBUNTU_MIRROR=
RUN if [ -n "$UBUNTU_MIRROR" ]; then \
      echo "🔄 配置镜像源: $UBUNTU_MIRROR" && \
      # 先使用官方源安装 ca-certificates（如果使用 HTTPS 镜像源需要）
      apt-get update -qq && \
      apt-get install -y --no-install-recommends ca-certificates && \
      # 配置镜像源
      sed -i "s|http://ports.ubuntu.com/ubuntu-ports|${UBUNTU_MIRROR}/ubuntu-ports|g" /etc/apt/sources.list && \
      sed -i "s|http://archive.ubuntu.com/ubuntu|${UBUNTU_MIRROR}/ubuntu|g" /etc/apt/sources.list && \
      sed -i "s|http://security.ubuntu.com/ubuntu|${UBUNTU_MIRROR}/ubuntu|g" /etc/apt/sources.list && \
      # 如果镜像源是 HTTPS，将 http:// 替换为 https://
      if echo "$UBUNTU_MIRROR" | grep -q "^https"; then \
        sed -i "s|http://${UBUNTU_MIRROR#https://}|${UBUNTU_MIRROR}|g" /etc/apt/sources.list; \
      fi && \
      rm -rf /var/lib/apt/lists/* && \
      echo "✅ 镜像源配置完成"; \
    else \
      echo "ℹ️  使用默认官方源"; \
    fi

# 配置 apt 重试机制和性能优化
RUN echo 'Acquire::Retries "3";' > /etc/apt/apt.conf.d/80-retries && \
    echo 'Acquire::http::Timeout "30";' >> /etc/apt/apt.conf.d/80-retries && \
    echo 'Acquire::ftp::Timeout "30";' >> /etc/apt/apt.conf.d/80-retries && \
    echo 'Acquire::https::Timeout "30";' >> /etc/apt/apt.conf.d/80-retries && \
    echo 'Acquire::http::MaxConnections "4";' >> /etc/apt/apt.conf.d/80-retries && \
    echo 'Acquire::Queue-Mode "host";' >> /etc/apt/apt.conf.d/80-retries && \
    echo 'APT::Get::Assume-Yes "true";' >> /etc/apt/apt.conf.d/80-retries && \
    echo 'APT::Get::Show-Upgraded "false";' >> /etc/apt/apt.conf.d/80-retries

# 安装依赖：优化 apt-get update 性能
# 注意：如果上面已经执行了 apt-get update（配置镜像源时），这里会使用缓存
RUN echo "🔄 更新包列表..." && \
    apt-get update -qq && \
    echo "✅ 包列表更新成功" && \
    apt-get install -y --fix-broken || true

# 安装基础开发工具
RUN echo "📦 安装基础工具..." && \
    apt-get update -qq && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --fix-missing --no-install-recommends \
      ca-certificates \
      build-essential \
      cmake \
      git \
      ninja-build \
      pkg-config \
      wget \
      curl && \
    rm -rf /var/lib/apt/lists/* && \
    # 验证安装
    command -v cmake >/dev/null 2>&1 || (echo "❌ cmake 安装失败" && exit 1) && \
    command -v make >/dev/null 2>&1 || (echo "❌ make 安装失败" && exit 1) && \
    echo "✅ 基础工具安装成功"

# 安装 Qt 6 和必要的开发库
RUN echo "安装 Qt6 开发库..." && \
    apt-get update -qq && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --fix-missing --no-install-recommends \
      libgles-dev \
      libglvnd-dev \
      qt6-base-dev \
      qt6-tools-dev \
      qt6-tools-dev-tools \
      qt6-l10n-tools \
      libqt6core6 \
      libqt6gui6 \
      libqt6widgets6 \
      libqt6network6 \
      libqt6printsupport6 \
      libgl1-mesa-dev \
      libglu1-mesa-dev \
      libxkbcommon-x11-0 \
      libxcb-icccm4 \
      libxcb-image0 \
      libxcb-keysyms1 \
      libxcb-randr0 \
      libxcb-render-util0 \
      libxcb-xinerama0 \
      libxcb-xfixes0 \
      libxcb-shape0 \
      libfontconfig1 \
      libdbus-1-3 \
      fonts-noto-cjk \
      fonts-wqy-microhei \
      fonts-wqy-zenhei && \
    apt-get install -y --fix-missing fonts-noto-cjk-extra || echo "Warning: fonts-noto-cjk-extra installation failed, continuing..." && \
    rm -rf /var/lib/apt/lists/* && \
    apt-get clean && \
    if command -v qmake6 >/dev/null 2>&1 && (find /usr -name "libQt6Core.so.6" -type f 2>/dev/null | grep -q . || dpkg -l | grep -q libqt6core6); then \
        echo "Qt6 库安装成功"; \
    else \
        echo "Qt6 库安装失败"; \
        exit 1; \
    fi

# 工作目录
WORKDIR /workspace

# Qt 默认平台和显示（可在 docker-compose 中覆盖）
ENV QT_QPA_PLATFORM=xcb
ENV DISPLAY=host.docker.internal:0

# 默认命令：进入 bash，方便交互
CMD ["/bin/bash"]

