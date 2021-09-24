use std::fmt::{self, Display};

/// Glosso Core Error Handler
#[derive(Debug)]
pub enum GlossoCoreErr {
    IOErr(std::io::Error),
    ParseMetadataErr(MetadataErrKind),
    ReadValueFailedErr,
    TryWriteIllegalValueErr,
}

/// Error kind related to the parsing metadata in the glosso binary file
#[derive(Debug)]
pub enum MetadataErrKind {
    InappropriateByteSize { expected: usize, got: usize },
    InvalidMagicNumber { got: u64 },
    VersionDiff { expected: u64, got: u64 },
}

impl From<std::io::Error> for GlossoCoreErr {
    fn from(err: std::io::Error) -> Self {
        Self::IOErr(err)
    }
}

impl Display for MetadataErrKind {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::InappropriateByteSize { expected, got } => write!(
                f,
                "to parse the metadata, at least {} bytes were given. But only {} bytes are given",
                expected, got
            ),
            Self::InvalidMagicNumber { got } => {
                write!(f, "invalid magic number found. got {}", got)
            }
            Self::VersionDiff { expected, got } => write!(
                f,
                "glosso version is different. expected {}, got {}",
                expected, got
            ),
        }
    }
}

impl Display for GlossoCoreErr {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::IOErr(err) => write!(f, "{}", err),
            Self::ParseMetadataErr(err) => write!(f, "{}", err),
            Self::ReadValueFailedErr => write!(f, "cannot read a Value from the given bytes"),
            Self::TryWriteIllegalValueErr => write!(
                f,
                "cannot write a given Value which cannot write into bytes"
            ),
        }
    }
}

pub type Result<T> = std::result::Result<T, GlossoCoreErr>;
