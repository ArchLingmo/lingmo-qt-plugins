# Maintainer: Lingmo OS Team <team@lingmo.org>
# Contributor: devalexandre <alexandre@dev2learn.com>
pkgname=lingmo-qt-plugins
pkgver=3.0.0
pkgrel=1
pkgdesc="Qt 6 platform theme and style plugins of the Lingmo desktop"
arch=("x86_64")
url="https://github.com/LingmoOS/lingmo-qt-plugins"
license=("GPL")
depends=("lingmoui" "qt6-base" "qt6-wayland" "kwindowsystem" "xcb-util-wm" "libx11")
makedepends=("cmake" "ninja" "extra-cmake-modules" "qt6-tools" "plasma-wayland-protocols" "git")
provides=("$pkgname")
conflicts=("$pkgname")
source=("git+$url.git")
sha512sums=("SKIP")

build() {
    cmake -S lingmo-qt-plugins -B build -G Ninja \
        -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib \
        -DCMAKE_BUILD_TYPE=None -DQT_NO_PRIVATE_MODULE_WARNING=ON
    cmake --build build
}

package() {
    DESTDIR="$pkgdir" cmake --install build
}
