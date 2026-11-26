def load_word_search(filename='input.txt'):
    """
    Load the word search grid from a file.
    
    Args:
        filename (str): Path to the input file containing the word search grid.
    
    Returns:
        list: 2D list representing the word search grid.
    """
    try:
        with open(filename, 'r') as file:
            # Read lines, strip whitespace, convert to list of characters
            grid = [list(line.strip()) for line in file if line.strip()]
        
        # Validate grid is rectangular
        if not grid:
            raise ValueError("Empty grid")
        
        row_length = len(grid[0])
        if not all(len(row) == row_length for row in grid):
            raise ValueError("Grid must be rectangular")
        
        return grid
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
        return None
    except Exception as e:
        print(f"Error reading file: {e}")
        return None

def count_xmas(grid):
    """
    Count all occurrences of 'XMAS' in the given grid.
    
    Args:
        grid (list): 2D list of characters representing the word search grid.
    
    Returns:
        int: Number of times 'XMAS' appears in the grid.
    """
    # Validate input
    if not grid or not grid[0]:
        return 0
    
    rows, cols = len(grid), len(grid[0])
    xmas_count = 0
    
    # Directions: right, down, diagonal down-right, diagonal down-left, 
    # and their reverse directions
    directions = [
        (0, 1),    # right
        (1, 0),    # down
        (1, 1),    # diagonal down-right
        (1, -1),   # diagonal down-left
        (0, -1),   # left
        (-1, 0),   # up
        (-1, -1),  # diagonal up-left
        (-1, 1)    # diagonal up-right
    ]
    
    def is_valid_pos(r, c):
        """Check if the position is within grid bounds."""
        return 0 <= r < rows and 0 <= c < cols
    
    def check_xmas(r, c, dr, dc):
        """
        Check if XMAS can be found starting from this position in this direction.
        
        Args:
            r (int): Starting row
            c (int): Starting column
            dr (int): Row direction increment
            dc (int): Column direction increment
        
        Returns:
            bool: True if XMAS is found, False otherwise
        """
        # 'XMAS' is 4 letters long
        target = list('XMAS')
        for i, letter in enumerate(target):
            curr_r, curr_c = r + i*dr, c + i*dc
            if not is_valid_pos(curr_r, curr_c) or grid[curr_r][curr_c] != letter:
                return False
        return True
    
    # Check every possible starting position and direction
    for r in range(rows):
        for c in range(cols):
            for dr, dc in directions:
                if check_xmas(r, c, dr, dc):
                    xmas_count += 1
    
    return xmas_count

def main():
    # Load word search grid from input.txt
    word_search = load_word_search()
    
    if word_search is not None:
        # Count XMAS occurrences
        result = count_xmas(word_search)
        print(f"Number of XMAS occurrences: {result}")

if __name__ == "__main__":
    main()