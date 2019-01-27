#include "wsfs/jsonrpc.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <pthread.h>

#include "wsfs/response_parser.h"

#define DEFAULT_TIMEOUT_SECS 10

struct wsfs_jsonrpc_response
{
	int id;
	wsfs_status status;
	json_t * result;
};

struct wsfs_jsonrpc
{
	pthread_mutex_t lock;
	pthread_cond_t finished;
	pthread_condattr_t finished_attributes;

	wsfs_create_message_fn * create_message;
	wsfs_send_message_fn * send_message;
	void * user_data;

	bool is_finished;
	struct wsfs_jsonrpc_response response;	
};

static json_t * wsfs_request_create(
	char const * method,
	int id,
	char const * param_info,
	va_list args)
{
	json_t * request = json_object();
	json_object_set_new(request, "method", json_string(method));
	json_t * params = json_array();
	
	for (char const * param_type = param_info; '\0' != *param_type; param_type++)
	{
		switch(*param_type)
		{
			case 's':
			{
				char const * const value = va_arg(args, char const *);
				json_array_append_new(params, json_string(value));
			}
			break;
			case 'i':
			{
				int const value = va_arg(args, int);
				json_array_append_new(params, json_integer(value));
			}
			break;
			default:
			fprintf(stderr, "fatal: unknown param_type '%c'\n", *param_type);
			exit(EXIT_FAILURE);
			break;
		}
	}
	

	json_object_set_new(request, "params", params);
	json_object_set_new(request, "id", json_integer(id));

	return request;
}


wsfs_status wsfs_jsonrpc_invoke(
	struct wsfs_jsonrpc * rpc,
	json_t * * result,
	char const * method,
	char const * param_info,
	...
)
{
	// enqueue message
	pthread_mutex_lock(&rpc->lock);
	wsfs_status status = WSFS_BAD;

	char * message = NULL;
	size_t length = 0;
	if (-1 == rpc->response.id) {

		va_list args;
		va_start(args, param_info);
		json_t * request = wsfs_request_create(method, 42, param_info, args);	
		va_end(args);

		
		length = json_dumpb(request, NULL, 0, JSON_COMPACT);
		if (0 < length)
		{	
			rpc->is_finished = false;	
			rpc->response.id = 42;
			rpc->response.result = NULL;
			rpc->response.status = WSFS_GOOD;

			message = rpc->create_message(length);
			json_dumpb(request, message, length, JSON_COMPACT);

		}
		json_decref(request);
	}
	pthread_mutex_unlock(&rpc->lock);
	

	if (NULL != message)
	{
		bool const success = rpc->send_message(message, length, rpc->user_data);

		// wait for answer
		pthread_mutex_lock(&rpc->lock);

		if (success)
		{
			struct timespec timeout;
			clock_gettime(CLOCK_MONOTONIC, &timeout);
			timeout.tv_sec += DEFAULT_TIMEOUT_SECS;
			int rc = 0;
			while ((0 == rc) && (!rpc->is_finished)) {
				rc = pthread_cond_timedwait(&rpc->finished, &rpc->lock, &timeout);
			}

			if (rpc->is_finished)
			{
				status = rpc->response.status;
				*result = rpc->response.result;
			}
			else
			{
				status = WSFS_BAD_TIMEOUT;
			}
		}
		
		rpc->response.id = -1;
		rpc->response.result = NULL;
		rpc->response.status = WSFS_GOOD;
		pthread_mutex_unlock(&rpc->lock);
	}

	return status;
}

struct wsfs_jsonrpc * wsfs_jsonrpc_create(
	wsfs_create_message_fn * create_message,
	wsfs_send_message_fn * send_message,
	void * user_data)
{
	struct wsfs_jsonrpc * rpc = malloc(sizeof(struct wsfs_jsonrpc));
	if (NULL != rpc)
	{
		pthread_mutex_init(&rpc->lock, NULL);

		pthread_condattr_init(&rpc->finished_attributes);
		pthread_condattr_setclock(&rpc->finished_attributes, CLOCK_MONOTONIC);
		pthread_cond_init(&rpc->finished, &rpc->finished_attributes);

		rpc->create_message = create_message;
		rpc->send_message = send_message;
		rpc->user_data = user_data;
		rpc->is_finished = true;
		rpc->response.id = -1;
		rpc->response.status = WSFS_GOOD;
		rpc->response.result = NULL;
	}

	return rpc;
}

void wsfs_jsonrpc_set_user_data(
	struct wsfs_jsonrpc * rpc,
	void * user_data)
{
	rpc->user_data = user_data;
}

void wsfs_jsonrpc_dispose(
	struct wsfs_jsonrpc * rpc)
{
	if (NULL != rpc->response.result)
	{
		json_decref(rpc->response.result);
	}

	pthread_cond_destroy(&rpc->finished);	
	pthread_condattr_destroy(&rpc->finished_attributes);
	pthread_mutex_destroy(&rpc->lock);
	

	free(rpc);
}

void wsfs_jsonrpc_on_message(
	char const * message,
	size_t length,
	void * user_data)
{
	struct wsfs_jsonrpc * rpc = user_data;

	struct wsfs_response response;
	wsfs_response_parse(message, length, &response);

	if (-1 != response.id)
	{
		pthread_mutex_lock(&rpc->lock);

		if (response.id == rpc->response.id)
		{
			rpc->is_finished = true;
			rpc->response.status = response.status;
			rpc->response.result = response.result;
		}
		else
		{
			if (NULL != response.result)
			{
				json_decref(response.result);
			}
		}

		pthread_cond_signal(&rpc->finished);
		pthread_mutex_unlock(&rpc->lock);
	}
}

