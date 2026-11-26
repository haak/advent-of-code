const std = @import("std");

pub fn main() !void {
    // Open the file
    const file = try std.fs.cwd().openFile("puppy.torrent", .{});
    defer file.close();

    // Read entire file into memory
    const allocator = std.heap.page_allocator;
    const contents = try file.readToEndAlloc(allocator, std.math.maxInt(usize));
    defer allocator.free(contents);

    // Print contents as UTF-8 string
    std.debug.print("{s}\n", .{contents});
}
