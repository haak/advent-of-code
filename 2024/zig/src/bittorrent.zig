const std = @import("std");
const Allocator = std.mem.Allocator;

// --- Bencoding ---

const BencodeValue = struct {
    kind: enum {
        integer,
        string,
        list,
        dictionary,
    },
    integer_value: i64 = 0,
    string_value: []const u8 = "", // needs to be allocated and freed after use
    list_value: []BencodeValue = &[_]BencodeValue{}, // same as string_value
    dictionary_value: std.StringHashMap(BencodeValue), // same as string_value

    pub fn init(allocator: Allocator) !BencodeValue {
        return BencodeValue{
            .kind = .string,
            .string_value = "",
            .dictionary_value = std.StringHashMap(BencodeValue).init(allocator),
            .list_value = &[_]BencodeValue{},
        };
    }

    pub fn deinit(self: *BencodeValue, allocator: Allocator) void {
        switch (self.kind) {
            .string => allocator.free(self.string_value),
            .list => {
                for (self.list_value) |item| {
                    item.deinit(allocator);
                }
                allocator.free(self.list_value);
            },
            .dictionary => {
                var it = self.dictionary_value.iterator();
                while (it.next()) |entry| {
                    entry.value_ptr.?.deinit(allocator);
                }
                self.dictionary_value.deinit();
            },
            else => {},
        }
    }
};

fn bdecode(allocator: Allocator, bencoded: []const u8) !BencodeValue {
    var index: usize = 0;
    return bdecodeInternal(allocator, bencoded, &index);
}

fn bdecodeInternal(allocator: Allocator, bencoded: []const u8, index: *usize) !BencodeValue {
    var value: BencodeValue = try BencodeValue.init(allocator);
    defer value.deinit(allocator);
    // defer if (std.debug.runtime_error) value.deinit(allocator);

    if (bencoded[index.*] == 'i') {
        value.kind = .integer;
        index.* += 1;
        const num_start: usize = index.*;
        while (bencoded[index.*] != 'e') {
            index.* += 1;
            if (index.* >= bencoded.len) return error.UnexpectedEndOfBencodedString;
        }
        const num_str: []const u8 = bencoded[num_start..index.*];
        value.integer_value = try std.fmt.parseInt(i64, num_str, 10);
        index.* += 1; // consume 'e'
    } else if (std.ascii.isDigit(bencoded[index.*])) {
        value.kind = .string;
        const len_start: usize = index.*;
        while (bencoded[index.*] != ':') {
            index.* += 1;
            if (index.* >= bencoded.len) return error.UnexpectedEndOfBencodedString;
        }
        const len_str: []const u8 = bencoded[len_start..index.*];
        const len = try std.fmt.parseInt(usize, len_str, 10);
        index.* += 1; // consume ':'

        const string_start: usize = index.*;
        index.* += len;
        if (index.* > bencoded.len) return error.UnexpectedEndOfBencodedString;
        value.string_value = try allocator.alloc(u8, len);
        @memcpy(value.string_value, bencoded[string_start..index.*]);
    } else if (bencoded[index.*] == 'l') {
        value.kind = .list;
        index.* += 1;
        var list = std.ArrayList(BencodeValue).init(allocator);
        defer list.deinit();

        while (bencoded[index.*] != 'e') {
            const item = try bdecodeInternal(allocator, bencoded, index);
            try list.append(item);
        }
        index.* += 1; // consume 'e'
        value.list_value = try list.toOwnedSlice();
    } else if (bencoded[index.*] == 'd') {
        value.kind = .dictionary;
        index.* += 1;
        while (bencoded[index.*] != 'e') {
            // Key (must be a string)
            if (!std.ascii.isDigit(bencoded[index.*])) return error.InvalidDictionaryKey;
            const key = try bdecodeInternal(allocator, bencoded, index);
            if (key.kind != .string) return error.InvalidDictionaryKey;

            // Value
            const val = try bdecodeInternal(allocator, bencoded, index);

            try value.dictionary_value.put(key.string_value, val); // copying the value
            key.deinit(allocator);
        }
        index.* += 1; // consume 'e'
    } else {
        return error.InvalidBencodedString;
    }

    return value;
}

fn bencodeInteger(allocator: Allocator, value: i64) ![]const u8 {
    return try std.fmt.allocPrint(allocator, "i{d}e", .{value});
}

fn bencodeString(allocator: Allocator, value: []const u8) ![]const u8 {
    return try std.fmt.allocPrint(allocator, "{d}:{s}", .{ value.len, value });
}

// --- Torrent Info ---

const TorrentInfo = struct {
    announce_url: []const u8,
    piece_length: u64,
    pieces_hashes: []const u8,
    name: []const u8,
    length: u64,

    pub fn init(allocator: Allocator, torrent_file_path: []const u8) !TorrentInfo {
        const file = try std.fs.readFileAlloc(allocator, torrent_file_path, std.math.maxInt(usize));
        defer allocator.free(file);

        const bencoded_data = try bdecode(allocator, file);
        defer bencoded_data.deinit(allocator);

        if (bencoded_data.kind != .dictionary) {
            return error.InvalidTorrentFile;
        }

        const announce_value = bencoded_data.dictionary_value.get("announce") orelse return error.MissingAnnounceUrl;
        if (announce_value.kind != .string) return error.InvalidAnnounceUrl;
        const announce_url = announce_value.string_value;

        const info_value = bencoded_data.dictionary_value.get("info") orelse return error.MissingInfoDict;
        if (info_value.kind != .dictionary) return error.InvalidInfoDict;

        const piece_length_value = info_value.dictionary_value.get("piece length") orelse return error.MissingPieceLength;
        if (piece_length_value.kind != .integer) return error.InvalidPieceLength;
        const piece_length = @as(u64, @intCast(piece_length_value.integer_value));

        const pieces_value = info_value.dictionary_value.get("pieces") orelse return error.MissingPieces;
        if (pieces_value.kind != .string) return error.InvalidPieces;
        const pieces_hashes = pieces_value.string_value;

        const name_value = info_value.dictionary_value.get("name") orelse return error.MissingName;
        if (name_value.kind != .string) return error.InvalidName;
        const name = name_value.string_value;

        const length_value = info_value.dictionary_value.get("length") orelse return error.MissingLength;
        if (length_value.kind != .integer) return error.InvalidLength;
        const length = @as(u64, @intCast(length_value.integer_value));

        return .{
            .announce_url = announce_url,
            .piece_length = piece_length,
            .pieces_hashes = pieces_hashes,
            .name = name,
            .length = length,
        };
    }

    pub fn deinit(self: *TorrentInfo, allocator: Allocator) void {
        allocator.free(self.announce_url);
    }
};

// --- Tracker Communication ---

fn urlEncode(allocator: std.mem.Allocator, input: []const u8) ![]u8 {
    var encoded = std.ArrayList(u8).init(allocator);
    defer encoded.deinit();

    for (input) |char| {
        if (std.ascii.isAlnum(char) or char == '-' or char == '_' or char == '.' or char == '~') {
            try encoded.append(char);
        } else {
            // Percent-encode other characters
            const encodedChar = try std.fmt.allocPrint(allocator, "%{x:02}", .{char});
            defer allocator.free(encodedChar);
            try encoded.append('%');
            try encoded.appendSlice(encodedChar);
        }
    }

    return encoded.toOwnedSlice();
}

fn trackerRequest(allocator: std.mem.Allocator, announce_url: []const u8, info_hash: []const u8, peer_id: []const u8, port: u16, uploaded: u64, downloaded: u64, left: u64, event: []const u8) ![]u8 {
    // URL-encode the info_hash
    const encodedInfoHash = try urlEncode(allocator, info_hash);
    defer allocator.free(encodedInfoHash);

    // Build the URL with parameters
    const url = try std.fmt.allocPrint(allocator, "{s}?info_hash={s}&peer_id={s}&port={d}&uploaded={d}&downloaded={d}&left={d}&event={s}", .{ announce_url, encodedInfoHash, peer_id, port, uploaded, downloaded, left, event });
    defer allocator.free(url);

    std.debug.print("Tracker URL: {}\n", .{url});

    // Use std.http.Client to make the HTTP request
    var client = std.http.Client{ .allocator = allocator };
    defer client.deinit();

    const reader = try client.get(url, .{});
    defer reader.deinit();

    // Read the response body
    var body = std.ArrayList(u8).init(allocator);
    defer body.deinit();

    const response_reader = reader.reader();
    try body.reader().streamUntilEof(response_reader);

    return body.toOwnedSlice();
}

// --- Tracker Server (Basic) ---

fn handleTrackerRequest(allocator: Allocator) ![]const u8 {
    // TODO: Parse the HTTP GET request to extract the parameters.
    // For simplicity, let's assume the client sends the parameters directly.
    // In a real server, you'd need to parse the HTTP request properly.

    // Example: Dummy peer list (replace with actual peer data from a database)
    const peers = "d8:intervali1800e5:peersl" ++
        "d2:ip9:127.0.0.17:porti6881ee" ++
        "d2:ip9:127.0.0.17:porti6882ee" ++
        "e"; // Sample peer list

    return try std.fmt.allocPrint(allocator, "d{s}e", .{peers}); // Bencode the peer list
}

// --- Main ---

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    // Client Side
    const torrent_file_path = "example.torrent"; // Replace with your torrent file

    var torrent_info = try TorrentInfo.init(allocator, torrent_file_path);
    defer torrent_info.deinit(allocator);

    std.debug.print("Announce URL: {}\n", .{torrent_info.announce_url});
    std.debug.print("Piece Length: {}\n", .{torrent_info.piece_length});
    std.debug.print("File Name: {}\n", .{torrent_info.name});

    // Calculate the info hash
    var info_hash = [_]u8{0} ** 20; //this is a placeholder. You will need to compute the info_hash
    //const info_hash = calculateInfoHash(torrent_info.info_dict);

    const peer_id = "-ZY0001-123456789012"; // Your peer ID
    const port: u16 = 6881;
    const uploaded: u64 = 0;
    const downloaded: u64 = 0;
    const left: u64 = torrent_info.length;

    const tracker_response = try trackerRequest(allocator, torrent_info.announce_url, &info_hash, peer_id, port, uploaded, downloaded, left, "started");
    defer allocator.free(tracker_response);

    std.debug.print("Tracker Response: {}\n", .{tracker_response});

    //TODO: Parse tracker response

    // Server Side
    const listener = try std.net.listen(std.net.Address.parseIp4("0.0.0.0:6969").?);
    defer listener.close();

    std.debug.print("Tracker listening on port 6969...\n", .{});

    while (true) {
        const stream = try listener.accept();
        defer stream.close();

        std.debug.print("Accepted connection from {}\n", .{stream.remote_address.?});

        // Read request data
        var buffer: [4096]u8 = undefined;
        const bytes_read = try stream.readAll(&buffer);
        const request_data = buffer[0..bytes_read];

        // Handle tracker request and send response
        const response = try handleTrackerRequest(allocator, request_data);
        defer allocator.free(response);
        try stream.writeAll(response);
    }
}

// const std = @import("std");
// const Allocator = std.mem.Allocator;
const testing = std.testing;

// Import the Bencoding code from your main file (assuming it's in bittorrent.zig)
// const bittorrent = @import("bittorrent.zig");
// const BencodeValue = bittorrent.BencodeValue;
// const bdecode = bittorrent.bdecode;

// const std = @import("std");
// const Allocator = std.mem.Allocator;
// const testing = std.testing;

// Import the Bencoding code from your main file (assuming it's in bittorrent.zig)
// const bittorrent = @import("bittorrent.zig");
// const BencodeValue = bittorrent.BencodeValue;
// const bdecode = bittorrent.bdecode;

// Helper function to compare BencodeValue structs (recursive)
fn bencodeValuesEqual(a: BencodeValue, b: BencodeValue) bool {
    if (a.kind != b.kind) {
        return false;
    }

    switch (a.kind) {
        .integer => return a.integer_value == b.integer_value,
        .string => return std.mem.eql(u8, a.string_value, b.string_value),
        .list => {
            if (a.list_value.len != b.list_value.len) {
                return false;
            }
            for (a.list_value, 0..) |item, i| {
                if (!bencodeValuesEqual(item, b.list_value[i])) {
                    return false;
                }
            }
            return true;
        },
        .dictionary => {
            if (a.dictionary_value.count() != b.dictionary_value.count()) {
                return false;
            }

            var a_it = a.dictionary_value.iterator();
            while (a_it.next()) |a_entry| {
                const b_value = b.dictionary_value.get(a_entry.key_ptr.*) orelse return false; // Key exists in b?
                if (!bencodeValuesEqual(a_entry.value_ptr.?, b_value.*)) {
                    return false;
                }
            }
            return true;
        },
        else => return true, // Should not happen, but handle it
    }
}

// Test cases
test "bdecode integer" {
    const allocator = testing.allocator;
    const bencoded = "i123e";
    const expected_value: i64 = 123;

    const decoded = try bdecode(allocator, bencoded);
    defer decoded.deinit(allocator);

    try testing.expectEqual(@as(BencodeValue.Kind, .integer), decoded.kind);
    try testing.expectEqual(expected_value, decoded.integer_value);
}

test "bdecode string" {
    const allocator = testing.allocator;
    const bencoded = "5:hello";
    const expected_string = "hello";

    const decoded = try bdecode(allocator, bencoded);
    defer decoded.deinit(allocator);

    try testing.expectEqual(@as(BencodeValue.Kind, .string), decoded.kind);
    try testing.expect(std.mem.eql(u8, expected_string, decoded.string_value));
}

test "bdecode list" {
    const allocator = testing.allocator;
    const bencoded = "l4:spam4:eggse";

    const decoded = try bdecode(allocator, bencoded);
    defer decoded.deinit(allocator);

    try testing.expectEqual(@as(BencodeValue.Kind, .list), decoded.kind);
    try testing.expectEqual(@as(usize, 2), decoded.list_value.len);
    try testing.expect(std.mem.eql(u8, "spam", decoded.list_value[0].string_value));
    try testing.expect(std.mem.eql(u8, "eggs", decoded.list_value[1].string_value));
}

test "bdecode dictionary" {
    const allocator = testing.allocator;
    const bencoded = "d3:cow3:moo4:spam4:eggse";

    const decoded = try bdecode(allocator, bencoded);
    defer decoded.deinit(allocator);

    try testing.expectEqual(@as(BencodeValue.Kind, .dictionary), decoded.kind);
    try testing.expectEqual(@as(usize, 2), decoded.dictionary_value.count());
    const cow_value = decoded.dictionary_value.get("cow") orelse return error.KeyNotFound;
    const spam_value = decoded.dictionary_value.get("spam") orelse return error.KeyNotFound;
    try testing.expect(std.mem.eql(u8, "moo", cow_value.string_value));
    try testing.expect(std.mem.eql(u8, "eggs", spam_value.string_value));
}

test "bdecode nested list and dictionary" {
    const allocator = testing.allocator;
    const bencoded = "d1:kli1e1:vle1:aee";

    const decoded = try bdecode(allocator, bencoded);
    defer decoded.deinit(allocator);

    try testing.expectEqual(@as(BencodeValue.Kind, .dictionary), decoded.kind);

    const k_value = decoded.dictionary_value.get("k") orelse return error.KeyNotFound;
    try testing.expectEqual(@as(BencodeValue.Kind, .integer), k_value.kind);
    try testing.expectEqual(@as(i64, 1), k_value.integer_value);

    const v_value = decoded.dictionary_value.get("v") orelse return error.KeyNotFound;
    try testing.expectEqual(@as(BencodeValue.Kind, .list), v_value.kind);

    try testing.expect(std.mem.eql(u8, "a", v_value.list_value[0].string_value));
}
