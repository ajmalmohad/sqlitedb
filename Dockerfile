FROM ruby:3.1.0

WORKDIR /app

COPY Gemfile Gemfile.lock ./
RUN bundle install

CMD ["/bin/bash", "-c", "gcc -o main main.c && bundle exec rspec"]