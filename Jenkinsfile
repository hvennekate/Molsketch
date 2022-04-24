pipeline {
    agent any

    stages {
        // stage('Checkout') {
        //     steps {
        //         cleanWs()
        //         git branch: 'main', url: 'https://github.com/hvennekate/Molsketch.git'
        //     }
        // }
        // stage('Tests') {
        //     steps {
        //         dir('testbuild') {
        //             // sh 'qmake-qt5 ../tests -spec linux-g++ CONFIG+=debug CONFIG-=qml_debug CXXTEST_INCLUDE_PATH=/home/hendrik/Programme/cxxtest-4.4 CXXTEST_BIN_PATH=/home/hendrik/Programme/cxxtest-4.4 && /usr/bin/make qmake_all'
        //             // sh 'make'
        //             sh 'xvfb-run ./msktests'
        //         }
        //     }
        // }
        // stage('Package') {
        //     steps {
        //         sh 'git archive HEAD -o Molsketch-0.0.1-src.tar.gz'
        //     }
        //     post {
        //         success {
        //             archiveArtifacts artifacts: '*.tar.gz', followSymlinks: false
        //         }
        //     }
        // }
        // stage('WinBuild') {
        //     steps {
        //         dir("winbuild") {
        //             sh '/opt/Qt-5.15.1-mingw32-static/bin/qmake DEFINES+=THIRD_PARTY_LICENSES LIBS+=-L/opt/openbabel-3.1.1-static/bin/ INCLUDEPATH+=/opt/openbabel-3.1.1-static/include/openbabel3 ../Molsketch.pro'
        //             sh 'make'
        //         }
        //     }
        // }
        stage('WinRepo') {

        }
        stage('Sourceforge') {
            // upload
            // select default https://sourceforge.net/p/forge/documentation/Using%20the%20Release%20API/
        }
        stage('Github') {
            // update windows repo (adapt files as necessary)
        }
        stage('Homepage') {
            // Update homepage
            // Add blog post https://anypoint.mulesoft.com/apiplatform/sourceforge/#/portals/organizations/98f11a03-7ec0-4a34-b001-c1ca0e0c45b1/apis/32951/versions/34322
        }
    }
}
