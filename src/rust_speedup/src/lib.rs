use clap::Parser;

/// Speedup measurement tool in Rust
#[derive(Parser, Debug)]
#[command(version, about)]
pub struct Args {
    /// Number of jobs to run 
    /// #[arg(value_parser = validate_count)]
    pub num_jobs: u32,

    /// Seed to run with, which determine the size of each job.
    pub seed: u32,
}

pub fn poly(x: f32) -> f32 {
    f32::powi(x, 3) - (4 as f32) * f32::powi(x, 2) + x
}

pub fn work(seed: u32) -> f32 {
    let mut s: f32 = 0.;
    let start: f32 = 0.;
    let end: f32 = 100.;
    let steps: u32 = seed * 1e8 as u32;
    let dx: f32 = (end - start) / (steps as f32);

    for i in 0..steps {
        s += poly((i as f32) * dx);
    }

    s * dx
}

