defineReplace(getVarDescription) {
  return($$section($$1, ;, 0, 0))
}

defineReplace(getVarName) {
  return($$section($$1, ;, 1, 1))
}

defineReplace(getVarDefault) {
  win* : return($$section($$1, ;, 3, 3))
  return($$section($$1, ;, 2, 2))
}
