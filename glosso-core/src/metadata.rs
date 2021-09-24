use crate::error::{self, GlossoCoreErr, MetadataErrKind};

pub const MAGIC_NUMBER: u64 = 0x01a46f73736f6c67;
pub const GLOSSO_VM_VERSION: u64 = 0x4148504c41;

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Metadata {
    magic: u64,
    version: u64,
    pub proc_location: u64,
    pub global_mem_length: u64,
}

impl Metadata {
    pub fn parse_metadata(data: &[u8]) -> error::Result<Self> {
        if data.len() < std::mem::size_of::<Self>() {
            return Err(GlossoCoreErr::ParseMetadataErr(
                MetadataErrKind::InappropriateByteSize {
                    expected: std::mem::size_of::<Self>(),
                    got: data.len(),
                },
            ));
        }

        // SAFETY: We checked that data contains at least size_of::<Metadata>() bytes.
        // In addition, we want to initialize metadata from raw memory bytes.
        // As Metadata is repr C and each members of Metadata has same types,
        // no additional padding is given: read from raw memory is safe.
        let metadata: Self = unsafe { std::ptr::read_unaligned(data.as_ptr() as *const _) };

        if metadata.magic != MAGIC_NUMBER {
            return Err(GlossoCoreErr::ParseMetadataErr(
                MetadataErrKind::InvalidMagicNumber {
                    got: metadata.magic,
                },
            ));
        }

        if metadata.version != GLOSSO_VM_VERSION {
            return Err(GlossoCoreErr::ParseMetadataErr(
                MetadataErrKind::VersionDiff {
                    expected: GLOSSO_VM_VERSION,
                    got: metadata.version,
                },
            ));
        }

        Ok(metadata)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::mem::{transmute, MaybeUninit};

    #[test]
    fn is_well_parse_metadata() -> error::Result<()> {
        let expected = Metadata {
            magic: MAGIC_NUMBER,
            version: GLOSSO_VM_VERSION,
            proc_location: 0xfa,
            global_mem_length: 0xce,
        };

        const BUFFER_SIZE: usize = std::mem::size_of::<Metadata>() + 12;

        let mut buffer: [u8; BUFFER_SIZE] = unsafe {
            let uninit: [MaybeUninit<u8>; BUFFER_SIZE] = MaybeUninit::uninit().assume_init();
            transmute::<_, [u8; BUFFER_SIZE]>(uninit)
        };

        unsafe {
            std::ptr::copy_nonoverlapping(
                &expected as *const Metadata as *const _,
                buffer.as_mut_ptr(),
                std::mem::size_of::<Metadata>(),
            );
            std::ptr::write_bytes(buffer.as_mut_ptr().add(BUFFER_SIZE - 12), 0xaa, 12);
        }

        println!("Buffer: {:x?}", buffer);

        let got = Metadata::parse_metadata(&buffer)?;

        assert_eq!(expected.proc_location, got.proc_location);
        assert_eq!(expected.global_mem_length, got.global_mem_length);

        Ok(())
    }
}
