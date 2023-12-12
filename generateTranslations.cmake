function(addTranslationResources _resource_file resourceFileName)
  set(qmDir "${CMAKE_CURRENT_BINARY_DIR}/.qm")
  file(MAKE_DIRECTORY ${qmDir})

  file(GLOB_RECURSE translationFiles *.ts)
  set_source_files_properties(${translationFiles} PROPERTIES OUTPUT_LOCATION ${qmDir})
  qt_add_translation(qmFileList ${translationFiles})

  set(qmResourcesFile "${qmDir}/${resourceFileName}.qrc")
  file(WRITE ${qmResourcesFile} "<!DOCTYPE RCC><RCC version=\"1.0\">\n  <qresource prefix=\"i18n\">\n")

  foreach(qm_file ${qmFileList})
    file(RELATIVE_PATH qm_filename ${qmDir} ${qm_file})
    file(APPEND ${qmResourcesFile} "    <file alias=\"${qm_filename}\">${qm_file}</file>\n")
  endforeach(qm_file)

  file(APPEND ${qmResourcesFile} "  </qresource>\n</RCC>\n")
  set(${_resource_file} ${qmResourcesFile} PARENT_SCOPE)
endfunction(addTranslationResources)
