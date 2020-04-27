
//pub mod os;
use crate::io;
use crate::kernel_malloc;

#[macro_export]
macro_rules! c_str {
	($arg:expr) => (concat!($arg, '\x00'))
}

#[macro_export]
macro_rules! print {
	// Static (zero-allocation) implementation that uses compile-time `concat!()` only
	($fmt:expr) => ({
		let msg = c_str!($fmt);
		let ptr = msg.as_ptr() as *const crate::raw::c_char; 
		unsafe {
			use crate::kernel_malloc;
			kernel_malloc::uprintf(ptr);
		}
	});
	
	// Dynamic implementation that processes format arguments
	($fmt:expr, $($arg:tt)*) => ({
		use ::core::fmt::Write;
		use io::KernelDebugWriter;
		
		let mut writer = KernelDebugWriter {};
		writer.write_fmt(format_args!($fmt, $($arg)*)).unwrap();
	});
}

#[macro_export] 
macro_rules! println {
	($fmt:expr)              => (print!(concat!($fmt, "\n")));
	($fmt:expr, $($arg:tt)+) => (print!(concat!($fmt, "\n"), $($arg)*));
}
