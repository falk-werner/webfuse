#!/usr/bin/env bats
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

load test_helper

setup() {
  PROJECTPATH="$(readlink -f "$BATS_TEST_DIRNAME/..")"
  BINPATH="$PROJECTPATH/bin"
  PATH="$BINPATH:$PATH"
  
  GROOVY=( groovy3 )
}

@test "groovy3 has required deps" {
  "${GROOVY[@]}" --version
}

print_fixture_ymlslurper() {
  cat <<- EOF
	import groovy.yaml.YamlSlurper
	
	def configYaml = '''\
	|---
	|application: "Sample App"
	|users:
	|- name: "mrhaki"
	|  likes:
	|  - Groovy
	|  - Clojure
	|  - Java
	|- name: "Hubert"
	|  likes:
	|  - Apples
	|  - Bananas
	|connections:
	|- "WS1"
	|- "WS2"
	'''
	
	// Parse the YAML.
	def config = new YamlSlurper().parseText(configYaml.stripMargin())
	 
	assert config.application == 'Sample App'
	 
	assert config.users.size() == 2
	assert config.users[0] == [name: 'mrhaki', likes: ['Groovy', 'Clojure', 'Java']]
	assert config.users[1] == [name: 'Hubert', likes: ['Apples', 'Bananas']]
	 
	assert config.connections == ['WS1', 'WS2']  
EOF
}

@test "groovy3 ymlslurper test" {
  run "${GROOVY[@]}" -e "$(print_fixture_ymlslurper)"
  
  printf -- '%s\n' "${lines[@]}"
  [ "$status" -eq 0 ]
  [ -z "${lines[@]}" ]
}



