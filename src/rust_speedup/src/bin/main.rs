use clap::Parser;
use rust_speedup;
use rust_speedup::Args;
use std::thread;
use std::time::Instant;
use cpu_time::ProcessTime;

fn main() {
    let args  = Args::parse();

    println!("Running {} jobs with a seed of {}.", args.num_jobs, args.seed);

    // Sync jobs
    let mut sync_results = vec![];
    let sync_wall_clock_start = Instant::now();
    let sync_cpu = ProcessTime::now();
    for _ in 0..args.num_jobs {
        sync_results.push(rust_speedup::work(args.seed));
    }

    let sync_wall_clock_duration = (Instant::now() - sync_wall_clock_start).as_secs_f32();
    let sync_cpu_duration = sync_cpu.elapsed().as_secs_f32();
    println!("sync wall clock duration: {sync_wall_clock_duration}", );
    println!("sync cpu duration: {sync_cpu_duration}");

    // Parallel jobs
    let mut handles = vec![];
    let mut async_results = vec![];
    let async_wall_clock_start = Instant::now();
    let async_cpu = ProcessTime::now();
    for _ in 0..args.num_jobs {
        let handle = thread::spawn(move || {
            rust_speedup::work(args.seed)
        });
        handles.push(handle);
    }

    for handle in handles {
        async_results.push(handle.join().unwrap());
    }

    let async_wall_clock_duration = (Instant::now() - async_wall_clock_start).as_secs_f32();
    let async_cpu_duration = async_cpu.elapsed().as_secs_f32();
    let speedup = sync_wall_clock_duration / async_wall_clock_duration;

    println!("async wall clock duration: {async_wall_clock_duration}");
    println!("async cpu duration: {async_cpu_duration}");
    println!("speedup: {speedup}");
}
