#!/bin/bash

fuse -g 2x --auto-load --no-confirm-actions --no-accelerate-loader --no-traps --no-fastload --tape dist/tap/program.tap
