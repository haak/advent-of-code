const std = @import("std");
const Allocator = std.mem.Allocator;
const ArrayList = std.ArrayList;

const BencodeValue = union(enum) {
    integer: i64,
    string: []const u8,
    list: ArrayList(BencodeValue),
    dictionary: std.StringArrayHashMap(BencodeValue),

    fn deinit(self: *BencodeValue, allocator: Allocator) void {
        switch (self.*) {
            .string => |s| allocator.free(s),
            .list => |*l| {
                for (l.items) |*item| {
                    item.deinit(allocator);
                }
                l.deinit();
            },
            .dictionary => |*d| {
                var it = d.iterator();
                while (it.next()) |entry| {
                    allocator.free(entry.key_ptr.*);
                    entry.value_ptr.deinit(allocator);
                }
                d.deinit();
            },
            else => {},
        }
    }
};

const BencodeError = error{
    InvalidFormat,
    UnexpectedEndOfInput,
    OutOfMemory,
};

fn parseBencode(allocator: Allocator, input: []const u8) BencodeError!BencodeValue {
    var index: usize = 0;
    return try parseValue(allocator, input, &index);
}

fn parseValue(allocator: Allocator, input: []const u8, index: *usize) BencodeError!BencodeValue {
    if (index.* >= input.len) return BencodeError.UnexpectedEndOfInput;

    return switch (input[index.*]) {
        'i' => try parseInteger(allocator, input, index),
        'l' => try parseList(allocator, input, index),
        'd' => try parseDictionary(allocator, input, index),
        '0'...'9' => try parseString(allocator, input, index),
        else => BencodeError.InvalidFormat,
    };
}

fn parseInteger(allocator: Allocator, input: []const u8, index: *usize) BencodeError!BencodeValue {
    _ = allocator; // Unused in this function
    if (input[index.*] != 'i') return BencodeError.InvalidFormat;
    index.* += 1;

    const start = index.*;
    while (index.* < input.len and input[index.*] != 'e') {
        index.* += 1;
    }
    if (index.* >= input.len) return BencodeError.UnexpectedEndOfInput;

    const integer_str = input[start..index.*];
    index.* += 1; // Skip 'e'

    const integer = std.fmt.parseInt(i64, integer_str, 10) catch return BencodeError.InvalidFormat;
    return BencodeValue{ .integer = integer };
}

fn parseString(allocator: Allocator, input: []const u8, index: *usize) BencodeError!BencodeValue {
    const start = index.*;
    while (index.* < input.len and input[index.*] != ':') {
        index.* += 1;
    }
    if (index.* >= input.len) return BencodeError.UnexpectedEndOfInput;

    const length_str = input[start..index.*];
    index.* += 1; // Skip ':'

    const length = std.fmt.parseInt(usize, length_str, 10) catch return BencodeError.InvalidFormat;
    if (index.* + length > input.len) return BencodeError.UnexpectedEndOfInput;

    const string = try allocator.dupe(u8, input[index.* .. index.* + length]);
    index.* += length;

    return BencodeValue{ .string = string };
}

fn parseList(allocator: Allocator, input: []const u8, index: *usize) BencodeError!BencodeValue {
    if (input[index.*] != 'l') return BencodeError.InvalidFormat;
    index.* += 1;

    var list = ArrayList(BencodeValue).init(allocator);
    errdefer {
        for (list.items) |*item| {
            item.deinit(allocator);
        }
        list.deinit();
    }

    while (index.* < input.len and input[index.*] != 'e') {
        const value = try parseValue(allocator, input, index);
        try list.append(value);
    }
    if (index.* >= input.len) return BencodeError.UnexpectedEndOfInput;

    index.* += 1; // Skip 'e'
    return BencodeValue{ .list = list };
}

fn parseDictionary(allocator: Allocator, input: []const u8, index: *usize) BencodeError!BencodeValue {
    if (input[index.*] != 'd') return BencodeError.InvalidFormat;
    index.* += 1;

    var dict = std.StringArrayHashMap(BencodeValue).init(allocator);
    errdefer {
        var it = dict.iterator();
        while (it.next()) |entry| {
            allocator.free(entry.key_ptr.*);
            entry.value_ptr.deinit(allocator);
        }
        dict.deinit();
    }

    while (index.* < input.len and input[index.*] != 'e') {
        var key = try parseString(allocator, input, index);
        if (key != .string) return BencodeError.InvalidFormat;

        const value = try parseValue(allocator, input, index);
        try dict.put(try allocator.dupe(u8, key.string), value);
    }
    if (index.* >= input.len) return BencodeError.UnexpectedEndOfInput;

    index.* += 1; // Skip 'e'
    return BencodeValue{ .dictionary = dict };
}

pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    const input = "d3:foo3:bar4:spami42ee";
    const result = try parseBencode(allocator, input);
    // print restult
    std.debug.print("Parsed Bencode: {}\n", .{result});

    defer result.deinit(allocator);

    std.debug.print("Parsed Bencode: {}\n", .{result});
}

test "parse integer" {
    const allocator = std.testing.allocator;

    // Test valid integer
    const input = "i42e";
    const result = try parseBencode(allocator, input);
    defer result.deinit(allocator);

    try std.testing.expectEqual(BencodeValue{ .integer = 42 }, result);

    // Test invalid integer
    try std.testing.expectError(BencodeError.InvalidFormat, parseBencode(allocator, "iabcde"));
}
