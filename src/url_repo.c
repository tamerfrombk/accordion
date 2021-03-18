#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <url_repo.h>
#include <log.h>

static char generate_random_char() {
    FILE *f = fopen("/dev/urandom", "r");
    if (f == NULL) {
        fatal("unable to generate random character\n");
    }

    char c = '0'; // start off with a non-letter value
    while (!isalpha(c)) {
        debug("read %c from /dev/urandom\n", c);
        c = fgetc(f);
    }
    fclose(f);

    return c;
}

void url_repo_init(url_repo_t *repo, int port)
{
    repo->port = port;

    // TODO: make this customizable
    repo->connection = redisConnect("localhost", 6379);
    if (repo->connection == NULL) {
        fatal("unable to connect to redis on %s:%d\n", "localhost", 6379);
    }
    if (repo->connection->err != 0) {
        fatal("error while connection to redis: %s\n", repo->connection->errstr);
    }
}

void url_repo_teardown(url_repo_t *repo)
{
    redisFree(repo->connection);
}

char *fetch_or_create_accordion_url(url_repo_t *repo, const char *url)
{
    char *accordion_url = fetch_accordion_url(repo, url);
    if (accordion_url == NULL) {
        debug("unable to fetch accordion url for %s\n", url);

        return create_accordion_url(repo, url);
    }

    return accordion_url;
}

char *fetch_accordion_url(url_repo_t *repo, const char *url)
{
    debug("fetching accordion url for %s\n", url);

    redisReply *redis_reply = redisCommand(repo->connection, "HGET accordion %s", url);
    if (redis_reply == NULL) {
        error("unable to query redis for the accordion url of '%s'\n", url);
        return NULL;
    }

    debug("redis reply type: %d str: %s\n", redis_reply->type, redis_reply->str);

    char *reply = NULL;
    if (redis_reply->type == REDIS_REPLY_STRING) {
        reply = strdup(redis_reply->str);
        if (reply == NULL) {
            error("could not allocate memory for redis reply\n");
        }
    } else if (redis_reply->type == REDIS_REPLY_NIL) {
        // no data to be found
    } else {
        error("unknown redis reply for %d type\n", redis_reply->type);
    }
    freeReplyObject(redis_reply);

    return reply;
}

char *create_accordion_url(url_repo_t *repo, const char *url)
{
    debug("creating accordion url for %s\n", url);

    char *accordion_url = calloc(256, sizeof(*accordion_url));
    if (accordion_url == NULL) {
        error("unable to acquire memory for accordion_url");
        return NULL;
    }

    char suffix[10] = {0};
    for (unsigned int i = 0; i < sizeof(suffix) - 1; ++i) {
        suffix[i] = generate_random_char();
    }

    char *hostname = fetch_hostname();
    if (hostname == NULL) {
        error("unable to resolve hostname\n");
        return NULL;
    }

    snprintf(accordion_url, 256, "http://%s:%d/g/%s", hostname, repo->port, suffix);

    free(hostname);

    redisReply *redis_reply = redisCommand(repo->connection, "HSET accordion %s %s %s %s", url, accordion_url, accordion_url, url);
    if (redis_reply == NULL) {
        error("unable to set accordion url for url '%s'\n", url);
        return NULL;
    }

    debug("redis reply type: %d str: %s\n", redis_reply->type, redis_reply->str);

    if (redis_reply->type == REDIS_REPLY_INTEGER) {
        debug("%d objects inserted\n", redis_reply->integer);
    } else {
        error("unknown redis reply for %d type\n", redis_reply->type);
    }
    freeReplyObject(redis_reply);

    return accordion_url;
}

char *fetch_long_url(url_repo_t *repo, const char *accordion_url)
{
    debug("fetching long url for accordion URL %s\n", accordion_url);

    redisReply *redis_reply = redisCommand(repo->connection, "HGET accordion %s", accordion_url);
    if (redis_reply == NULL) {
        error("unable to query redis for the long url of '%s'\n", accordion_url);
        return NULL;
    }

    debug("redis reply type: %d str: %s\n", redis_reply->type, redis_reply->str);

    char *reply = NULL;
    if (redis_reply->type == REDIS_REPLY_STRING) {
        reply = strdup(redis_reply->str);
        if (reply == NULL) {
            error("could not allocate memory for redis reply\n");
        }
    } else if (redis_reply->type == REDIS_REPLY_NIL) {
        // no data to be found
    } else {
        error("unknown redis reply for %d type\n", redis_reply->type);
    }
    freeReplyObject(redis_reply);

    return reply;
}

char *fetch_hostname()
{
    FILE *f = fopen("/etc/hostname", "r");
    if (f == NULL) {
        debug("unable to open /etc/hostname\n");
        return NULL;
    }

    char *hostname = calloc(32, sizeof(*hostname));
    if (hostname == NULL) {
        debug("unable to allocate hostname memory\n");
        fclose(f);
        return NULL;
    }

    size_t n = fread(hostname, sizeof(*hostname), 32 - 1, f);
    fclose(f);

    hostname[n - 1] = '\0';

    return hostname;
}