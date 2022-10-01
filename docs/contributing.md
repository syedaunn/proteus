## Reporting bugs
- Please use the [GitHub issues tab](https://github.com/epfl-dias/proteus/issues) to report bugs.
- Use a clear and descriptive title for the issue to identify the problem.
- Describe the exact steps which reproduce the problem in as many details as possible. 
- Provide a detailed description of your environment. e.g kernel/distribution version, whether you are running in docker or on bare metal and which commit you are using.

# How to submit changes
Please open a PR on the GitHub repo. Please follow our git conventions, see the git section below.

## Runtime Environment
### Hugepages
Proteus requires Linux hugepages. To allocate hugepages:
```sh
echo 76800 | sudo tee /sys/devices/system/node/node{0,1}/hugepages/hugepages-2048kB/nr_hugepages
```
You may need to vary the number of huge pages based on your systems memory. You may also need to change `node{0,1}` based on the number of numa nodes in your system. 

## Development Environment
See [building](building.md) on how to build Proteus.

### Git
#### Conventions
We follow a fairly standard set of git conventions:
- We maintain a linear history. Please rebase on main before opening a pull request. 
- Please keep individual commits to manageable sizes. Each commit should be as self-contained as possible and under 500 lines.
- Commit messages should have a title starting with a tag, e.g `[storage] move storage into its own library`  (The current list of tags can be found in `.githooks/commit-msg`).
- Any non-trivial commit should have a message body detailing the reasoning/background of the changes, e.g describing pros/cons of alternate approaches, how the committer decided on this approach or links to external documentation/bug trackers where appropriate. 

#### Setting up git hooks

To setup the git hooks run:
```sh
git config core.hooksPath .githooks
```
This enables a variety of automatic notifications and configurations at commit-time, including formatting your committed code, checking conformance with licenses, worktree skip-lists, etc.

Furthermore, it's highly recommended to run the following to use our predefined git config:
```sh
git config --local include.path .config/diascld/.gitconfig
```


## Testing
For our c++ code we use [GTest](https://github.com/google/googletest) for our functional testing and [googlebenchmark](https://github.com/google/benchmark) for performance testing.
cpp integration tests live in `tests`. cpp Unit tests are in `tests` subdirectories in the component they test, and likewise performance tests are in `perftests` subdirectories.
The planner uses junit. 