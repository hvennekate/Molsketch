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
        script {
          env.msk_version = readFile 'version'
          env.msk_version -= '\n'
          env.msk_version_nick = readFile 'versionnick'
          env.msk_version_nick -= '\n'
        }
      }
    }
    stage('print') {
      steps {
        echo "Build version:  ${env.msk_version_nick} ${env.msk_version}"
      }
    }
  }
}
