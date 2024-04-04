FROM ruby:3.1.0

WORKDIR /app

COPY Gemfile ./

RUN bundle install && apt-get update && apt-get install -y gcc make