#!/bin/bash
bundle install
chmod +x runtest.sh
gcc -o main main.c
bundle exec rspec