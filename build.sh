CURRNET_PATH=`pwd`
THIRD_PARTY=../../third

function clean() {
    (cd ${CURRNET_PATH}/make && make clean)
    rm -rf ${CURRNET_PATH}/include
    rm -rf ${CURRNET_PATH}/lib

	echo "\033[1;32mDone!\033[0m"
}

function build() {
    if ! test -e ${THIRD_PARTY}; then
        git clone https://github.com/zhnzh2015/third-c ${THIRD_PARTY}
    fi

    (cd ${THIRD_PARTY}/gtest && sh build.sh 1.7.0)

    (cd ${CURRNET_PATH}/make && make publish)
	echo "\033[1;32mDone!\033[0m"
}

case $1 in
    clean)
        clean
    ;;

    *)
        build
    ;;

esac
