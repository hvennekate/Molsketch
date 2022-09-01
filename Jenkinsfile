def obabeldir = '/opt/openbabel-3.1.1-static';
def mingwdir = '/usr/i686-w64-mingw32/sys-root/mingw/bin'
def openssldir = '/opt/openssl-1.1.1n-mingw32/bin'
def blogUrl= ''

pipeline {
  agent any
  // TODO determine version and check it does not yet exist
  // TODO push tag to github

  stages {
    stage('Version') {
      steps {
        dir('sources') {
          script {
            env.msk_version = readFile 'version'
            env.msk_version -= '\n'
            env.msk_version_nick = readFile 'versionnick'
            env.msk_version_nick -= '\n'
          }
          echo "Build version:  ${env.msk_version_nick} ${env.msk_version}"
        }
      }
    }
    stage('Build') {
      steps {
        dir('mainbuild') {
          sh '''
            qmake-qt5 ../sources/Molsketch.pro \
            CONFIG+=release
          '''
          sh 'make -j 8'
        }
      }
    }
    stage('Test') {
      steps {
        dir('testbuild') {
          sh '''
            qmake-qt5 ../sources/tests \
              -spec linux-g++ \
              CONFIG+=debug \
              CONFIG-=qml_debug \
              CXXTEST_INCLUDE_PATH=/opt/cxxtest-4.4 \
              CXXTEST_BIN_PATH=/opt/cxxtest-4.4
          '''
          sh 'make -j 8'
          sh 'xvfb-run ./msktests'
        }
      }
      post {
        always {
          dir('testbuild') {
            junit checksName: 'Test report collection', keepLongStdio: true, testResults: 'TEST-cxxtest.xml'
          }
        }
      }
    }
    stage('Package') {
      steps {
        dir('sources') {
          sh 'git archive HEAD -o Molsketch-0.0.1-src.tar.gz'
        }
      }
      post {
        success {
          archiveArtifacts artifacts: 'sources/*.tar.gz', followSymlinks: false
        }
      }
    }
    stage('WinBuild') {
      steps {
        dir('winbuild') {
          sh '''
            /opt/Qt-5.15.5-mingw32-static/bin/qmake \
            CONFIG-=debug CONFIG+=release \
            DEFINES+=THIRD_PARTY_LICENSES \
            OB_LIBRARY_DIRS+=/opt/openbabel-3.1.1-static/lib/ \
            OB_INCLUDE_DIRS+=/opt/openbabel-3.1.1-static/include/openbabel3 \
            ../sources/Molsketch.pro
          '''
          sh 'make -j 8'
        }
      }
      post {
        success {
          dir('winbuild') {
            stash includes: 'bin/molsketch.exe', name: 'mainExecutable'
            stash includes: 'lib/obabeliface.dll', name: 'obabeliface'
          }
        }
      }
    }
    stage('WinPackage') {
      steps {
        dir('sources/wininstaller/packages/org.molsketch/data') {
          unstash 'mainExecutable'
        }
        dir('sources/wininstaller/packages/org.molsketch.openbabel.adapter/data') {
          unstash 'obabeliface'
        }
        dir(obabeldir) {
          fileOperations([
            folderCreateOperation("${WORKSPACE}/sources/wininstaller/packages/org.openbabel.formats/data"),
            fileCopyOperation(flattenFiles: true, includes: "**/*.obf", targetLocation: "${WORKSPACE}/sources/wininstaller/packages/org.openbabel.formats/data"),
            folderCreateOperation("${WORKSPACE}/sources/wininstaller/packages/org.openbabel.mainlib/data"),
            fileCopyOperation(flattenFiles: true, includes: "**/*.dll", targetLocation: "${WORKSPACE}/sources/wininstaller/packages/org.openbabel.mainlib/data"),
          ])
        }
        dir(mingwdir) {
          fileOperations([
            fileCopyOperation(flattenFiles: true, includes: '**/libstdc++-6.dll', targetLocation: "${WORKSPACE}/sources/wininstaller/packages/org.openbabel.mainlib/data/"),
            fileCopyOperation(flattenFiles: true, includes: '**/libgcc_s_sjlj-1.dll', targetLocation: "${WORKSPACE}/sources/wininstaller/packages/org.openbabel.mainlib/data/"),
            fileCopyOperation(flattenFiles: true, includes: '**/libwinpthread-1.dll', targetLocation: "${WORKSPACE}/sources/wininstaller/packages/org.openbabel.mainlib/data/"),
          ])
        }
        dir(openssldir) {
          fileOperations([
            folderCreateOperation("${WORKSPACE}/sources/wininstaller/packages/org.openssl.lib/data/"),
            fileCopyOperation(flattenFiles: true, includes: '**/*.dll', targetLocation: "${WORKSPACE}/sources/wininstaller/packages/org.openssl.lib/data/")
          ])
        }
        dir("sources/wininstaller") {
          sh '/opt/qt-installer-fw-win/bin/binarycreator.exe -c config/config.xml -p packages -f MolsketchInstaller.exe'
        }
      }
    }
  }
}
