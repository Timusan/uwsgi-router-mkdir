#include <uwsgi.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>

static int mkdir_p(const char *path) {
  /* Adapted from
     https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950
  */
  const size_t len = strlen(path);
  char _path[PATH_MAX];
  char *p;

  errno = 0;

  // Copy string so its mutable
  if (len > sizeof(_path)-1) {
    errno = ENAMETOOLONG;

    return -1;
  }
  strcpy(_path, path);

  // Iterate the string
  for (p = _path + 1; *p; p++) {
    if (*p == '/') {
      // Temporarily truncate
      *p = '\0';

      if (mkdir(_path, S_IRWXU) != 0) {
	if (errno != EEXIST)
	  return -1;
      }

      *p = '/';
    }
  }

  if (mkdir(_path, S_IRWXU) != 0) {
    if (errno != EEXIST)
      return -1;
  }

  return 0;
}

static int router_mkdir_func(struct wsgi_request *wsgi_req, struct uwsgi_route *ur) {
        char **subject = (char **) (((char *)(wsgi_req))+ur->subject);
        uint16_t *subject_len = (uint16_t *)  (((char *)(wsgi_req))+ur->subject_len);

        struct uwsgi_buffer *ub = uwsgi_routing_translate(wsgi_req, ur, *subject, *subject_len, ur->data, ur->data_len);
        if (!ub) return UWSGI_ROUTE_BREAK;

	// Recursively create the wanted directory structure
	// abusing slashes in the URL as directory separators.
	// Note: ub->buf is \0 terminated, so it is safe for mkdir()
        mkdir_p(ub->buf);

        uwsgi_buffer_destroy(ub);
        return UWSGI_ROUTE_NEXT;
}

static int router_mkdir(struct uwsgi_route *ur, char *args) {
        ur->func = router_mkdir_func;
        ur->data = args;
        ur->data_len = strlen(args);
        return 0;
}

static void register_router_mkdir(void) {
        uwsgi_register_router("mkdir", router_mkdir);
}

struct uwsgi_plugin router_mkdir_plugin = {
        .name = "router_mkdir",
        .on_load = register_router_mkdir,
};
