/*
Copyright (c) 2012-2014 The SSDB Authors. All rights reserved.
Use of this source code is governed by a BSD-style license that can be
found in the LICENSE file.
*/
#include "worker.h"
#include "link.h"
#include "proc.h"
#include "../util/log.h"
#include "../include.h"

ProcWorker::ProcWorker(const std::string &name){
	this->name = name;
}

void ProcWorker::init(){
	log_debug("%s %d init", this->name.c_str(), this->id);
}

int ProcWorker::proc(ProcJob *job){
	const Request *req = job->req;
	
	proc_t p = job->cmd->proc;
	job->time_wait = 1000 * (millitime() - job->stime);
	job->result = (*p)(job->serv, job->link, *req, &job->resp);
	job->time_proc = 1000 * (millitime() - job->stime) - job->time_wait;

//	if (breplication && job->cmd->flags & Command::FLAG_WRITE){
		//todo 将req中的命令和参数保存值buffer中，待全量复制结束时发送值从ssdb
//	}

	if(job->link->send(job->resp.resp) == -1){

		log_debug("job->link->send error");
		job->result = PROC_ERROR;
		return 0;
	}

	//todo append custom reply

	int len = job->link->write();
	if(len < 0){
		log_debug("job->link->write error");
		job->result = PROC_ERROR;
	}

	return 0;
}
