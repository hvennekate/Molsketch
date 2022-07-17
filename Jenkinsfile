def obabeldir = '/opt/openbabel-3.1.1-static';
def mingwdir = '/usr/i686-w64-mingw32/sys-root/mingw/bin'
def openssldir = '/opt/openssl-1.1.1j-mingw32/bin'
def blogUrl= ''

pipeline {
  agent any
  // TODO determine version and check it does not yet exist
  // TODO push tag to github

  stages {
    stage('Checkout') {
      steps {
        // cleanWs()
        // TODO change branch to main
        git branch: 'Jenkins_setup', url: 'git@github.com:hvennekate/Molsketch.git', credentialsId: 'github'
      }
    }
    stage('Version') {
      steps {
        script {
          env.msk_version = readFile 'version'
          env.msk_version -= '\n'
          env.msk_version_nick = readFile 'versionnick'
          env.msk_version_nick -= '\n'
        }
        echo "Build version:  ${env.msk_version_nick} ${env.msk_version}"
      }
    }
    stage('Build') {
      steps {
        dir('mainbuild') {
          sh '''
            qmake-qt5 ../Molsketch.pro \
            CONFIG+=release
          '''
          sh 'make'
        }
      }
    }
    stage('Test') {
      steps {
        dir('testbuild') {
          sh '''
            qmake-qt5 ../tests \
              -spec linux-g++ \
              CONFIG+=debug \
              CONFIG-=qml_debug \
              CXXTEST_INCLUDE_PATH=/opt/cxxtest-4.4 \
              CXXTEST_BIN_PATH=/opt/cxxtest-4.4
          '''
          sh 'make'
          sh 'xvfb-run ./msktests TextActionAcceptanceTest'
        }
        post {
          always {
            dir('testbuild') {
              junit checksName: 'Test report collection', keepLongStdio: true, testResults: 'TEST-cxxtest.xml'
            }
          }
        }
      }
    }
    stage('Package') {
      steps {
        sh 'git archive HEAD -o Molsketch-0.0.1-src.tar.gz'
      }
      post {
        success {
          archiveArtifacts artifacts: '*.tar.gz', followSymlinks: false
        }
      }
    }
  }
}
