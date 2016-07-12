# uwsgi-router-mkdir
A uWSGI router extension which allows you to make a directory recursivly if it doesn't exist yet.

## Usage

When defining a route you have the ability to create a directory / directories. For instance

```ini
route = .* mkdir:/tmp/uwsgi_cache${PATH_INFO}
```

If PATH_INFO translates to '/foo/bar/' the following directory structure will be made:

```sh
/tmp/uwsgi_cache/foo/bar/
```
Given that uWSGI has permission to do so of course.

## Flat file caching with expiration

This plugin is part of a set of tiny plugins which aims to give the user flat file caching with sane directory structure and file expiration.

uWSGI is fully capable of storing responses into flat files, but storing them into a single directory on a site with a large amount of pages could lead to file system problems.

Storing the responses in files scattered throughout directories circumvents these issues.

However, randomly named directories would make it very hard to find which URL/page is cached in which file.

Given that your site has a sane URL layout, this simple plugin in combination with the PATH_INFO variable gives you the ability to have your URL structure reflected on disk and thus make it much easier to retrieve/delete a cached response for a certain page without hitting file system problems.

Other plugins in this set:

- [uWSGI isfreshfile](https://github.com/Timusan/uwsgi-router-isfreshfile)

With the above plugin you can invalidate files older than a certain age making for cache-style expiration behavior.

Complete usage example:

```ini
# Check if the cached file is fresh (86400 = 24 hours), if so serve it directly.
route-if = isfreshfile:/tmp/uwsgi_cache${PATH_INFO}file.html,86400 static:/tmp/uwsgi_cache${PATH_INFO}file.html
# Not fresh or does not exist? Create the full directory structure first.
route = .* mkdir:/tmp/uwsgi_cache${PATH_INFO}
# And (re)create the cached response file.
route-run = tofile:filename=/tmp/uwsgi_cache${PATH_INFO}file.html
```

Big thanks to [Roberto](https://gist.github.com/rdeioris) for his support!
