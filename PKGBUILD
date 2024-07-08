pkgname=wayfire-window-borders
pkgver=0.1
pkgrel=1
pkgdesc="Simple window borders for Wayfire."
arch=('x86_64')
url="https://github.com/allomanta/wayfire-window-borders"
license=('MIT')
depends=('wayfire')
makedepends=('git' 'meson' 'ninja' 'cmake')
provides=('wayfire-window-borders')
conflicts=("$pkgname")
replaces=()
options=()

source=()
sha256sums=()

build() {
    cd ../
    meson build --prefix=/usr --buildtype=release
    ninja -C build
}


package() {
    cd ../
    DESTDIR="$pkgdir/" ninja -C build install
}
