/*
 * Remote processor messaging
 *
 * Copyright(c) 2012 Texas Instruments. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name Texas Instruments nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LINUX_RPMSG_RESMGR_H
#define _LINUX_RPMSG_RESMGR_H

/*
 * enum rprm_action - RPMSG resource manager actions
 * @RPRM_CONNECT:	RESMGR channel creation.
 * @RPRM_DISCONNECT:	RESMGR channel destroy.
 * @RPRM_REQUEST:	resource request from the client.
 * @RPRM_RELEASE:	resource release from the client.
 * @RPRM_SET_CONSTRAINTS: set a constraint for a resource.
 * @RPRM_RPRM_CLEAR_CONSTRAINTS: clear a constraint for a resource.
 *
 */
enum rprm_action {
	RPRM_CONNECT		= 0,
	RPRM_DISCONNECT		= 1,
	RPRM_REQUEST		= 2,
	RPRM_RELEASE		= 3,
	RPRM_SET_CONSTRAINTS	= 4,
	RPRM_CLEAR_CONSTRAINTS	= 5,
	RPRM_REQ_DATA		= 6,
};

/*
 * enum rprm_constraint_type - RPMSG constraint types
 * @RPRM_SCALE:		request a scale constraint
 * @RPRM_LATENCY:	request a latency constraint
 * @RPRM_BANDWIDTH:	request a bandwidth constraint
 *
 */
enum rprm_constraint_type {
	RPRM_SCALE		= 0x1,
	RPRM_LATENCY		= 0x2,
	RPRM_BANDWIDTH		= 0x4,
};

/*
 * enum rprm_request_data_type - RPMSG request data types
 * @RPRM_MAX_FREQ:	request maximum frequency
 *
 */
enum rprm_request_data_type {
	RPRM_MAX_FREQ		= 0,
};

/**
 * struct rprm_res_ops - resource-specific device operations
 * @start:		request a resource
 * @stop:		release a resource
 * @get_info:		get properties of the resource into a buffer
 * @scale:		set a scale constraint
 * @latency:		set a latency constraint
 * @bandwidth:		set a bandwidth constraint
 * @get_max_freq:	get the maximum frequency of the resuource
 */
struct rprm_res_ops {
	int (*request)(void **handle, void *args, size_t len);
	int (*release)(void *handle);
	int (*get_info)(void *handle, char *buf, size_t len);
	int (*scale)(struct device *rdev, void *handle, unsigned long val);
	int (*latency)(struct device *rdev, void *handle, unsigned long val);
	int (*bandwidth)(struct device *rdev, void *handle, unsigned long val);
	unsigned long (*get_max_freq)(void *handle);
};

/**
 * struct rprm_res - resource manager resource structure
 * @next:	pointer to the next resource
 * @name:	name of the resource
 * @ower:	module owner of the resource
 * @ops:	pointer to the operations of the resource
 */
struct rprm_res {
	struct list_head next;
	const char *name;
	struct module *owner;
	struct rprm_res_ops *ops;
};

/**
 * struct rprm_constraints_data - resource manager constraint data
 * @mask:	state which constraints are valid
 * @frequency:	value of the frequency constraint
 * @bandwidth:	value of the bandwidth constraint
 * @latency:	value of the latency constraint
 */
struct rprm_constraints_data {
	u32 mask;
	long frequency;
	long bandwidth;
	long latency;
};

/**
 * struct rprm_request - header of a request action
 * @res_name:	name of the resource
 * @data:	additional information needed by below layer (parameters)
 */
struct rprm_request {
	char res_name[16];
	char data[];
} __packed;

/**
 * struct rprm_release - header of a release action
 * @res_id:	id of the resource
 */
struct rprm_release {
	u32 res_id;
} __packed;

/**
 * struct rprm_constraint - header of a constraint action
 * @res_id:	id of the resource
 * @cdata:	constraint information
 */
struct rprm_constraint {
	u32 res_id;
	struct rprm_constraints_data cdata;
} __packed;

/**
 * struct rprm_constraint - header of a constraint action
 * @res_id:	id of the resource
 * @type:	type of requested data
 * @data:	pointer to get the requested data
 */
struct rprm_request_data {
	u32 res_id;
	u32 type;
	char data[];
} __packed;

/**
 * struct rprm_msg - header for all the actions
 * @action:	action requested
 * @data:	addition information depending on @action
 */
struct rprm_msg {
	u32 action;
	char data[];
} __packed;

/**
 * struct rprm_msg - header for respond and action
 * @res_id:	resource id
 * @base:	resource base address (da address of the resource)
 * @data:	additional information returned to the client
 */
struct rprm_request_ack {
	u32 res_id;
	u32 base;
	char data[];
} __packed;

/**
 * struct rprm_msg - header for respond and action
 * @ret:	status value returned by rmsg resmgr server
 * @data:	additional information returned to the client
 */
struct rprm_ack {
	u32 action;
	u32 ret;
	char data[];
} __packed;

int rprm_resource_register(struct rprm_res *res);
int rprm_resource_unregister(struct rprm_res *res);
#endif /* _LINUX_RPMSG_RESMGR_H */
