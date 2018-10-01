/* Tetris Kit - Tetris.c
 __________ __________  __   ____
/_   _/  -_)_   _/  - )(__)_/  _/
 /__/ \___/ /__//__\__/__/____/
Copyright (C) 2000-2018 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU Lesser General Public License v3. */

#include <Z/functions/base/Z2D.h>

#ifndef TETRIS_STATIC
#	define TETRIS_API Z_API_EXPORT
#endif

#ifdef TETRIS_USE_LOCAL_HEADER
#	include "Tetris.h"
#else
#	include <games/puzzle/Tetris.h>
#endif

#ifdef TETRIS_USE_C_STANDARD_LIBRARY
#	include <stdlib.h>
#	include <string.h>

#	define z_deallocate(block)			  free(block)
#	define z_reallocate(block, block_size)		  realloc(block, block_size)
#	define z_move(block, block_size, output)	  memmove(output, block, block_size)
#	define z_block_int8_set(block, block_size, value) memset(block, value, block_size)

#	define z_block_int8_find_value(block, block_size, value) \
		memchr(block, value, block_size)
#else
#	include <ZBase/allocation.h>
#	include <ZBase/block.h>
#endif

#define HIT_SIDE   1
#define HIT_BOTTOM 2

#define ____ 0

TETRIS_API TetrisPiece const tetris_pieces[7][4] = {
	{{{{{____}, {____}, {____}, {____}},
	   {{____}, {0x91}, {0xC1}, {____}},
	   {{____}, {0x31}, {0x61}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {2, 2}},
	 {{{{____}, {____}, {____}, {____}},
	   {{____}, {0x91}, {0xC1}, {____}},
	   {{____}, {0x31}, {0x61}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {2, 2}},
	 {{{{____}, {____}, {____}, {____}},
	   {{____}, {0x91}, {0xC1}, {____}},
	   {{____}, {0x31}, {0x61}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {2, 2}},
	 {{{{____}, {____}, {____}, {____}},
	   {{____}, {0x91}, {0xC1}, {____}},
	   {{____}, {0x31}, {0x61}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {2, 2}}},

	{{{{{____}, {____}, {____}, {____}},
	   {{0xB2}, {0xA2}, {0xA2}, {0xE2}},
	   {{____}, {____}, {____}, {____}},
	   {{____}, {____}, {____}, {____}}}, {0, 1}, {3, 1}},
	 {{{{____}, {____}, {0xD2}, {____}},
	   {{____}, {____}, {0x52}, {____}},
	   {{____}, {____}, {0x52}, {____}},
	   {{____}, {____}, {0x72}, {____}}}, {2, 0}, {2, 3}},
	 {{{{____}, {____}, {____}, {____}},
	   {{0xB2}, {0xA2}, {0xA2}, {0xE2}},
	   {{____}, {____}, {____}, {____}},
	   {{____}, {____}, {____}, {____}}}, {0, 1}, {3, 1}},
	 {{{{____}, {____}, {0xD2}, {____}},
	   {{____}, {____}, {0x52}, {____}},
	   {{____}, {____}, {0x52}, {____}},
	   {{____}, {____}, {0x72}, {____}}}, {2, 0}, {2, 3}}},

	{{{{{____}, {____}, {____}, {____}},
	   {{____}, {____}, {0x93}, {0xE3}},
	   {{____}, {0xB3}, {0x63}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {3, 2}},
	 {{{{____}, {____}, {0xD3}, {____}},
	   {{____}, {____}, {0x33}, {0xC3}},
	   {{____}, {____}, {____}, {0x73}},
	   {{____}, {____}, {____}, {____}}}, {2, 0}, {3, 2}},
	 {{{{____}, {____}, {____}, {____}},
	   {{____}, {____}, {0x93}, {0xE3}},
	   {{____}, {0xB3}, {0x63}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {3, 2}},
	 {{{{____}, {____}, {0xD3}, {____}},
	   {{____}, {____}, {0x33}, {0xC3}},
	   {{____}, {____}, {____}, {0x73}},
	   {{____}, {____}, {____}, {____}}}, {2, 0}, {3, 2}}},

	{{{{{____}, {____}, {____}, {____}},
	   {{____}, {0xB4}, {0xC4}, {____}},
	   {{____}, {____}, {0x34}, {0xE4}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {3, 2}},
	 {{{{____}, {____}, {____}, {0xD4}},
	   {{____}, {____}, {0x94}, {0x64}},
	   {{____}, {____}, {0x74}, {____}},
	   {{____}, {____}, {____}, {____}}}, {2, 0}, {3, 2}},
	 {{{{____}, {____}, {____}, {____}},
	   {{____}, {0xB4}, {0xC4}, {____}},
	   {{____}, {____}, {0x34}, {0xE4}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {3, 2}},
	 {{{{____}, {____}, {____}, {0xD4}},
	   {{____}, {____}, {0x94}, {0x64}},
	   {{____}, {____}, {0x74}, {____}},
	   {{____}, {____}, {____}, {____}}}, {2, 0}, {3, 2}}},

	{{{{{____}, {____}, {____}, {____}},
	   {{____}, {0x95}, {0xA5}, {0xE5}},
	   {{____}, {0x75}, {____}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {3, 2}},
	 {{{{____}, {____}, {0xD5}, {____}},
	   {{____}, {____}, {0x55}, {____}},
	   {{____}, {____}, {0x35}, {0xE5}},
	   {{____}, {____}, {____}, {____}}}, {2, 0}, {3, 2}},
	 {{{{____}, {____}, {____}, {0xD5}},
	   {{____}, {0xB5}, {0xA5}, {0x65}},
	   {{____}, {____}, {____}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 0}, {3, 1}},
	 {{{{____}, {0xB5}, {0xC5}, {____}},
	   {{____}, {____}, {0x55}, {____}},
	   {{____}, {____}, {0x75}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 0}, {2, 2}}},

	{{{{{____}, {____}, {____}, {____}},
	   {{____}, {0xB6}, {0xA6}, {0xC6}},
	   {{____}, {____}, {____}, {0x76}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {3, 2}},
	 {{{{____}, {____}, {0x96}, {0xE6}},
	   {{____}, {____}, {0x56}, {____}},
	   {{____}, {____}, {0x76}, {____}},
	   {{____}, {____}, {____}, {____}}}, {2, 0}, {3, 2}},
	 {{{{____}, {0xD6}, {____}, {____}},
	   {{____}, {0x36}, {0xA6}, {0xE6}},
	   {{____}, {____}, {____}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 0}, {3, 1}},
	 {{{{____}, {____}, {0xD6}, {____}},
	   {{____}, {____}, {0x56}, {____}},
	   {{____}, {0xB6}, {0x66}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 0}, {2, 2}}},

	{{{{{____}, {____}, {____}, {____}},
	   {{____}, {0xB7}, {0x87}, {0xE7}},
	   {{____}, {____}, {0x77}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 1}, {3, 2}},
	 {{{{____}, {____}, {0xD7}, {____}},
	   {{____}, {____}, {0x17}, {0xE7}},
	   {{____}, {____}, {0x77}, {____}},
	   {{____}, {____}, {____}, {____}}}, {2, 0}, {3, 2}},
	 {{{{____}, {____}, {0xD7}, {____}},
	   {{____}, {0xB7}, {0x27}, {0xE7}},
	   {{____}, {____}, {____}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 0}, {3, 1}},
	 {{{{____}, {____}, {0xD7}, {____}},
	   {{____}, {0xB7}, {0x47}, {____}},
	   {{____}, {____}, {0x77}, {____}},
	   {{____}, {____}, {____}, {____}}}, {1, 0}, {2, 2}}}
};


static zuint8 bounds_hit(Tetris const *object, TetrisPiece const *piece, Z2DSInt8 piece_point)
	{
	return	(zsint)piece_point.x + piece->a.x < 0 ||
		(zsint)piece_point.x + piece->b.x >= object->size.x
			? HIT_SIDE
			: ((zsint)piece_point.y + piece->b.y >= object->size.y
				? HIT_BOTTOM : 0);
	}


static zboolean content_hit(Tetris const *object, TetrisPiece const *piece, Z2DSInt8 piece_point)
	{
	zsint x, y = (zsint)piece_point.y + piece->a.y < 0 ? -piece_point.y : piece->a.y;

	for (; y <= piece->b.y; y++) for (x = piece->a.x; x <= piece->b.x; x++) if (
		piece->matrix[y][x].value &&
		object->matrix[object->size.x * (piece_point.y + y) + piece_point.x + x].value
	)
		return TRUE;

	return FALSE;
	}


static void consolidate(Tetris *object)
	{
	zsint x, y;
	zuint8 v;
	TetrisPiece const *piece = object->piece;

	object->full_row_count = 0;

	for (y = piece->a.y; y <= piece->b.y; y++)
		{
		for (x = piece->a.x; x <= piece->b.x; x++) if ((v = piece->matrix[y][x].value))
			object->matrix[
				(object->piece_point.y + y) * object->size.x +
				 object->piece_point.x + x
			].value = v;

		if (NULL == z_block_int8_find_value
			(object->matrix + object->size.x * (object->piece_point.y + y),
			 (zusize)object->size.x, 0)
		)
			object->full_row_indices[object->full_row_count++]
			= (zsint8)(object->piece_point.y + y);
		}

	if (object->piece_point.y + piece->a.y < object->top)
		object->top = object->piece_point.y + piece->a.y;
	}


TETRIS_API void tetris_initialize(Tetris *object)
	{
	object->matrix = NULL;
	object->size   = z_2d_sint8_zero;
	}


TETRIS_API void tetris_finalize(Tetris *object)
	{z_deallocate(object->matrix);}


TETRIS_API ZStatus tetris_prepare(Tetris *object, Z2DSInt8 size, zuint8 next_piece_index)
	{
	zusize matrix_size;

	if (size.x < TETRIS_MINIMUM_SIZE_X || size.y < TETRIS_MINIMUM_SIZE_Y)
		return Z_ERROR_TOO_SMALL;

	if (	(zusize)object->size.x * (zusize)object->size.y !=
		(matrix_size = (zusize)size.x * (zusize)size.y)
	)
		{
		void *matrix = z_reallocate(object->matrix, matrix_size);

		if (matrix == NULL) return Z_ERROR_NOT_ENOUGH_MEMORY;
		object->matrix = matrix;
		}

	z_block_int8_set(object->matrix, matrix_size, 0);
	object->size		 = size;
	object->top		 = size.y;
	object->piece		 = NULL;
	object->full_row_count	 = 0;
	object->next_piece	 = tetris_pieces[next_piece_index];
	object->next_piece_index = next_piece_index;
	return Z_OK;
	}


TETRIS_API void tetris_insert_piece(Tetris *object, zuint8 next_piece_index)
	{
	object->piece_point.x	 = (object->size.x - 4) / 2;
	object->piece_point.y	 = -3;
	object->piece_rotation	 = 0;
	object->piece		 = object->next_piece;
	object->piece_index	 = object->next_piece_index;
	object->next_piece_index = next_piece_index;
	object->next_piece	 = tetris_pieces[next_piece_index];
	}


TETRIS_API TetrisResult tetris_move_piece(Tetris *object, TetrisDirection direction)
	{
	TetrisPiece const *piece = object->piece;

	Z2DSInt8 point = direction
		? z_2d_sint8(object->piece_point.x + direction, object->piece_point.y)
		: z_2d_sint8(object->piece_point.x, object->piece_point.y + 1);

	zuint8 hit = bounds_hit(object, piece, point);

	if (hit)
		{
		if (hit == HIT_SIDE) return TETRIS_RESULT_HIT;
		consolidate(object); return TETRIS_RESULT_CONSOLIDATED;
		}

	if (content_hit(object, piece, point))
		{
		if (direction) return TETRIS_RESULT_HIT;
		if (object->piece_point.y + piece->a.y < 0) return TETRIS_RESULT_GAME_OVER;
		consolidate(object);
		return TETRIS_RESULT_CONSOLIDATED;
		}

	object->piece_point = point;
	return Z_OK;
	}


TETRIS_API TetrisResult tetris_rotate_piece(Tetris *object, TetrisDirection direction)
	{
	zuint8 rotation = (object->piece_rotation + direction) & 3;
	TetrisPiece const *piece = &tetris_pieces[object->piece_index][rotation];

	if (	bounds_hit (object, piece, object->piece_point) ||
		content_hit(object, piece, object->piece_point)
	)
		return TETRIS_RESULT_HIT;

	object->piece_rotation = (zuint8)rotation;
	object->piece = piece;
	return Z_OK;
	}


TETRIS_API TetrisResult tetris_drop_piece(Tetris *object)
	{
	TetrisPiece const *piece = object->piece;
	Z2DSInt8 point = object->piece_point;

	do point.y++;
	while (!bounds_hit(object, piece, point) && !content_hit(object, piece, point));

	if ((object->piece_point.y = point.y - 1) + piece->a.y < 0)
		return TETRIS_RESULT_GAME_OVER;

	consolidate(object);
	return TETRIS_RESULT_CONSOLIDATED;
	}


#define ROWS(index) (matrix + x_size * (index))


TETRIS_API void tetris_remove_full_rows(Tetris *object)
	{
	zuint8 row_count = object->full_row_count;

	if (row_count)
		{
		zsint8 *full_row_indices = object->full_row_indices;
		zsint delta = 0, index, size, x_size = object->size.x;
		TetrisCell *matrix = object->matrix, *row, *row_above, *row_below, cell;

		while (row_count)
			{
			row = ROWS(full_row_indices[--row_count]);
			row_above = row - x_size;
			row_below = row + x_size;

			for (index = x_size; index;)
				{
				if (!(cell = row[--index]).fields.top_edge)
					row_above[index].fields.bottom_edge = TRUE;

				if (!cell.fields.bottom_edge)
					row_below[index].fields.top_edge = TRUE;
				}
			}

		for (index = row_count = object->full_row_count; --index;)
			{
			delta++;

			if ((size = full_row_indices[index] - full_row_indices[index - 1] - 1))
				z_move	(ROWS(full_row_indices[index - 1] + 1), x_size * size,
					 ROWS(full_row_indices[index - 1] + 1 + delta));
			}

		z_move	(ROWS(object->top), x_size * (full_row_indices[0] - object->top),
			 ROWS(object->top + row_count));

		z_block_int8_set(ROWS(object->top), x_size * row_count, 0);
		object->full_row_count = 0;
		object->top += row_count;
		}
	}


/* Tetris.c EOF */
