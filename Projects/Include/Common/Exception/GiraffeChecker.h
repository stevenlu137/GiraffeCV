/*
 * Copyright (c) 2015-2023 Pengju Lu, Yanli Wang

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
 
 
#ifndef GIRAFFECV_CHECKER_H_
#define GIRAFFECV_CHECKER_H_

#include "GiraffeException.h"

#define g_action_on_event(evt,act) if ((evt)) { act; }

#define g_on_event_exception(stat,excep) g_action_on_event((stat), { GLOGGER(error) << "Exception: "<<#stat<<" is not satisfied. "<<__FILE__<<__LINE__; throw excep; })

#define g_on_event_ret_false(stat) g_action_on_event((stat),{ GLOGGER(error) << "Error: "<<#stat<<" is not satisfied. "<<__FILE__<<__LINE__; return false; })

#define g_check_exit(stat) g_action_on_event(!(stat), { GLOGGER(error) << "Exception: "<<#stat<<" is not satisfied. "<<__FILE__<<__LINE__; exit(EXIT_FAILURE); })

#define g_check_exception(stat,excep) g_action_on_event(!(stat), { GLOGGER(error) << "Exception: "<<#stat<<" is not satisfied. "<<__FILE__<<__LINE__; throw excep; })

#define g_check_ret_false(stat) g_action_on_event(!(stat),{ GLOGGER(error) << "Error: "<<#stat<<" is not satisfied. "<<__FILE__<<__LINE__; return false; })

#define g_check_do_sth(stat,act) g_action_on_event(!(stat),{ act; })



#endif //GIRAFFECV_CHECKER_H_
