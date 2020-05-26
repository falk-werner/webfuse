#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

.PHONY: testhelper-default
testhelper-default:

.PHONY: testhelper-print-%
testhelper-print-%:
	@printf '%s\n' $($*)

.PHONY: testhelper-default-silent
testhelper-default-silent:
	$(SILENT)true

.PHONY: testhelper-recursive-print-%
testhelper-recursive-print-%:
	@$(MAKE) $(MFLAGS) $(addprefix -f,$(MAKEFILE_LIST)) testhelper-print-$* $(MAKEOVERRIDES)
