group "default" {
  targets = ["build"]
}

target "build" {
    dockerfile = "Dockerfile"
    context = "."
    targets = ["build"]
    output = ["out"]
}