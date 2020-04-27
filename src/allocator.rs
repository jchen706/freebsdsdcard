
//#![allow(warnings, unused_variables, dead_code, improper_ctypes, non_camel_case_types, non_snake_case, non_upper_case_globals)]

use core::alloc::{GlobalAlloc, Layout};
use core::fmt;
use core::ptr;
//use crate::mutex::Mutex;
use crate::kernel_malloc as kernel;
//use ::os::align_file::MIN_ALIGN;

use crate::raw;




pub struct FreebsdAllocator;



unsafe impl GlobalAlloc for FreebsdAllocator {

    unsafe fn alloc(&self, _layout: Layout)-> *mut u8 {   
       //check alignment: 

       return  kernel::malloc( _layout.size() as raw::c_size_t, &mut kernel::M_DEVBUF[0], kernel::M_WAITOK as i32) as *mut u8 
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        kernel::free(ptr as *mut raw::c_void, &mut kernel::M_DEVBUF[0]);
    }




}











