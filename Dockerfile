FROM ruby:3.1.0

WORKDIR /app

COPY Gemfile ./
RUN bundle install