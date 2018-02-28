#!/bin/bash

function clone_or_update_from_git() {
    GIT_REPO_URL=$1
    DIR=$2 
    if [ ! -d ${DIR} ]; then
        echo "git clone ${GIT_REPO_URL} ${DIR}"
        git clone ${GIT_REPO_URL} ${DIR}
    else 
        pushd ${DIR}
        echo "git pull origin master"
        git pull origin master 
        popd
    fi
}

function clone_or_update_from_git_with_submodule() {
    GIT_REPO_URL=$1
    DIR=$2 
    if [ ! -d ${DIR} ]; then
        echo "git clone ${GIT_REPO_URL} ${DIR}"
        git clone ${GIT_REPO_URL} ${DIR}
        git submodule init
        git submodule update
    else 
        pushd ${DIR}
        echo "git pull origin master"
        git pull origin master 
        git submodule init
        git submodule update
        popd
    fi
}

function build_project() {
    PROJECT=$1
    shift 1
    PROJECT_OPT=$*
    NAME=${PROJECT}-build 
    mkdir -p ${NAME} && cd ${NAME}
    echo "cmake ${PROJECT_OPT} -DCMAKE_INSTALL_PREFIX=../../ ../${PROJECT}"
    cmake ${PROJECT_OPT} -DCMAKE_INSTALL_PREFIX=../../ ../${PROJECT}
    echo "make VERBOSE=1 && make install"
    make VERBOSE=1 && make install 
    cd ../
}

function clear_build_history() {
    mkdir -p build/ && cd build 
    rm -rf *-build
    cd ../
}

function make_dep() {
    PROJECT_URL=$1
    PROJECT_NAME=$2

    shift 2
    PROJECT_OPT=$*
    mkdir -p build/ && cd build 
    
    echo "clone/update ${PROJECT_NAME} from ${PROJECT_URL}..."
    clone_or_update_from_git ${PROJECT_URL} ${PROJECT_NAME}

    echo "build ${PROJECT_NAME}..."
    build_project ${PROJECT_NAME} ${PROJECT_OPT}

    cd ../
}

function make_dep_with_submodule() {
    PROJECT_URL=$1
    PROJECT_NAME=$2

    shift 2
    PROJECT_OPT=$*
    mkdir -p build/ && cd build 
    
    echo "clone/update with submodule ${PROJECT_NAME} from ${PROJECT_URL}..."
    clone_or_update_from_git_with_submodule ${PROJECT_URL} ${PROJECT_NAME}

    echo "build ${PROJECT_NAME}..."
    build_project ${PROJECT_NAME} ${PROJECT_OPT}

    cd ../
}