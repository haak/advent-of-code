const std = @import("std");
const Allocator = std.mem.Allocator;
pub const BencodeValue = @import("bencode.zig").BencodeValue;
pub const parseBencode = @import("bencode.zig").parseBencode;

test "parse string" {
    const allocator = std.testing.allocator;

    // Test valid string
    const input = "4:spam";
    const result = try parseBencode(allocator, input);
    defer result.deinit(allocator);

    try std.testing.expectEqualStrings("spam", result.string);

    // Test invalid string (incorrect length)
    try std.testing.expectError(error.InvalidFormat, parseBencode(allocator, "5:spam"));
}

test "parse list" {
    const allocator = std.testing.allocator;

    // Test valid list
    const input = "l4:spami42ee";
    const result = try parseBencode(allocator, input);
    defer result.deinit(allocator);

    try std.testing.expect(result == .list);
    try std.testing.expectEqualStrings("spam", result.list.items[0].string);
    try std.testing.expectEqual(@as(i64, 42), result.list.items[1].integer);

    // Test invalid list (missing 'e')
    try std.testing.expectError(error.UnexpectedEndOfInput, parseBencode(allocator, "l4:spami42e"));
}

test "parse dictionary" {
    const allocator = std.testing.allocator;

    // Test valid dictionary
    const input = "d3:foo3:bar4:spami42ee";
    const result = try parseBencode(allocator, input);
    defer result.deinit(allocator);

    try std.testing.expect(result == .dictionary);
    try std.testing.expectEqualStrings("bar", result.dictionary.get("foo").?.string);
    try std.testing.expectEqual(@as(i64, 42), result.dictionary.get("spam").?.integer);

    // Test invalid dictionary (missing 'e')
    try std.testing.expectError(error.UnexpectedEndOfInput, parseBencode(allocator, "d3:foo3:bar4:spami42e"));
}

test "parse nested structures" {
    const allocator = std.testing.allocator;

    // Test nested list and dictionary
    const input = "d3:foo3:bar4:spaml1:a1:bee";
    const result = try parseBencode(allocator, input);
    defer result.deinit(allocator);

    try std.testing.expect(result == .dictionary);
    try std.testing.expectEqualStrings("bar", result.dictionary.get("foo").?.string);

    const spam_list = result.dictionary.get("spam").?.list;
    try std.testing.expectEqualStrings("a", spam_list.items[0].string);
    try std.testing.expectEqualStrings("b", spam_list.items[1].string);
}

test "parse invalid input" {
    const allocator = std.testing.allocator;

    // Test invalid input (empty string)
    try std.testing.expectError(error.InvalidFormat, parseBencode(allocator, ""));

    // Test invalid input (unexpected character)
    try std.testing.expectError(error.InvalidFormat, parseBencode(allocator, "x"));
}
