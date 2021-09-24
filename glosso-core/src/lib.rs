#![warn(missing_debug_implementations)]

//! # Core Part of the Glosso Language Implementation
//!
//! Glosso Core library is literally the core part the whole glosso language toolchains, such as olfactory
//! (a assembler to the glosso virtual assembly), glossovm (a virtual machine which can execute glosso assemble codes),
//! and glossoc (a compiler of the glosso language).

pub mod error;
pub mod instruction;
pub mod metadata;
pub mod pointer;
pub mod value;

mod utility;
