#pragma once

#include "bakkesmod/wrappers/wrapperstructs.h"

#include <string>
#include <vector>
#include <functional>

/**
 * JLG TODO Document Table
 */
class Table
{
	//--------------------STATIC VARIABLES----------------------
		public:
			enum class Align {
				Left,
				Top,
				Center,
				Right,
				Bottom
			};
			struct Cell
			{
				Cell( const std::string& value, LinearColor color, LinearColor backgroundColor, Table::Align hAlign = Table::Align::Center, Table::Align vAlign = Table::Align::Center )
					: value( value )
					, color( color )
					, backgroundColor( backgroundColor )
					, hAlign( hAlign )
					, vAlign( vAlign )
					, size( { 0.0f, 0.0f } )
					, valueSize( { 0.0f, 0.0f } )
					, valueOffset( { 0.0f, 0.0f } )
					, padTop( 0.0f )
					, padRight( 0.0f )
					, padBottom( 0.0f )
					, padLeft( 0.0f )
				{}

				std::string value;
				LinearColor color;
				LinearColor backgroundColor;

				Vector2F size;
				float padTop;
				float padRight;
				float padBottom;
				float padLeft;
				Vector2F valueSize;
				Vector2F valueOffset;

				Table::Align hAlign;
				Table::Align vAlign;

				Vector2F paddingSize() const
				{
					return Vector2F{ padLeft + padRight, padTop + padBottom };
				}
				Vector2F paddedSize() const
				{
					return size + paddingSize();
				}
				void setPadding( float pad )
				{
					padTop = pad;
					padRight = pad;
					padBottom = pad;
					padLeft = pad;
				}
				void setPadding( float h, float v )
				{
					padTop = v;
					padRight = h;
					padBottom = v;
					padLeft = h;
				}
				void setPadding( float t, float r, float b, float l )
				{
					padTop = t;
					padRight = r;
					padBottom = b;
					padLeft = l;
				}
			};
			typedef std::vector<Cell> Row;
			typedef std::vector<std::reference_wrapper<Cell>> Column;
			typedef std::vector<Row> Rows;

	//---------------------STATIC METHODS-----------------------

	//----------------------CONSTRUCTORS------------------------
	public:
		Table();
		Table( const Row& headings );
		virtual ~Table();

	//--------------------INSTANCE METHODS----------------------
	public:
		bool empty() const;
		size_t numRows() const;
		size_t numCols() const;

		Vector2F size() const;

		void addRow( const Row& row );
		void clear();
		void reset();

		void fitSize();
		void realign();

		void forEach( std::function<void( const Cell&, size_t r, size_t c)> f ) const;
		void forEach( std::function<void( Cell&, size_t r, size_t c)> f );

		void forEach( std::function<bool( const Cell&, size_t r, size_t c)> f ) const;
		void forEach( std::function<bool( Cell&, size_t r, size_t c)> f );

		Vector2F getCellOrigin( size_t r, size_t c ) const;

	private:
		Column column( const size_t& r );
		Row& row( const size_t& r );
		Row& operator[]( const size_t& r );

	//-------------------INSTANCE VARIABLES---------------------
	private:
		Rows table;
};