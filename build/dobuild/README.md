# dobuild

## Introduction

A major concern when developing and maintaining software, is to achieve reproducibility of builds. This can be 
especially necessary, when product service contract or warranty requirements enforce backports of bug fixes into 
previously released versions.

As noted by [reproducible-builds.org](https://reproducible-builds.org/docs/):

> Getting reproducible builds for your software might be easier than you think! But it might require small changes
> to your build system and a strategy on how to enable others to recreate an environment in which the builds can be
> reproduced.

The goal of this project is to support users, editing, building, testing and distributing software, by providing an 
*easy to install* and *auditable* deterministic build environment based on GNU Make and OCI Containers.

## Overview

TODO

## Prerequisites

TODO

## Embed dobuild in your project using git

A simple way of referencing dobuild in your standalone project is to use it as git subtree. Using git subtree allows to nest
one repository inside another as a sub-directory. It is one of several ways git can manage dependencies between repositories
(or projects). For further details refer to [Git subtree: the alternative to Git submodule](https://www.atlassian.com/blog/git/alternatives-to-git-submodule-git-subtree).

### Initial Setup

Change to the root of your standalone project git repository. Ensure repository has no outstanding changes, or reset them
(git reset --hard). Adding the subtree as a remote allows you to refer to it in shorter form (dobuild). After adding
the remote, add the subtree to a folder e.g. build/dobuild/ selecting a tag or branch e.g. master to clone the content of 
repository.

```bash

   git remote add -f dobuild https://github.com/nosamad/dobuild.git \
&& git subtree add --squash --prefix build/dobuild/ dobuild master

```

### Update changes

To update changes in sub-project dobuild, pull the subtree (build/dobuild/) selecting the branch or tag e.g. master again.

```bash

   git fetch dobuild master \
&& git subtree pull --squash --prefix build/dobuild/ dobuild master

```

Afterwards changes can be freely committed as part of your standalone project repository.

### Contribute changes back upstream

To contribute changes back to the upstream project (dobuild), your need to fork the project and add it as another remote:

```bash

git remote add dobuild-fork https://github.com/<YOURUSER>/dobuild.git

```

Afterwards you can use a subtree push to get your changes into your forked repository, in order to open a pull-request.

```bash

git subtree push --prefix=build/dobuild/ dobuild-fork master

```

### Purge

Purging a git subtree is also fairly easy - just remove the directory and commit the change. There are no additional config settings to purge unlike known from git submodules.

## Roadmap

TODO

## License

Copyright (c) 2019 Contributors as noted in the AUTHORS file.

Licensed under the [MPL-2](LICENSE.txt) License.
