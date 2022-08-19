#include "Table.h"

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
Table::Table()
{}

Table::Table( const Row& headings )
{
	addRow( headings );
}

Table::~Table()
{}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void Table::addRow( const Table::Row& row )
{
	if( table.size() != 0 && row.size() != table.front().size() )
		throw std::exception( "Table::addRow() : new row does not have correct number of columns" );

	table.push_back( row );
}

void Table::reset()
{
	table.clear();
}
void Table::clear()
{
	auto first = table.begin();
	std::advance( first, 1 );
	table.erase( first, table.end() );
}
bool Table::empty() const
{
	return (table.size() == 0 || table.front().size() == 0);
}
size_t Table::numRows() const
{
	if( table.empty() ) return 0;

	return table.size();
}
size_t Table::numCols() const
{
	if( table.empty() ) return 0;

	return table.front().size();
}

Table::Column Table::column( const size_t& r )
{
	Table::Column col;
	for( auto& row : table )
		col.push_back( std::ref(row[r]) );
	return col;
}
Table::Row& Table::row( const size_t& r )
{
	return table[r];
}
Table::Row& Table::operator[]( const size_t& r )
{
	return row(r);
}

Vector2F Table::size() const
{
	Vector2F size{ 0.0, 0.0 };
	if( empty() ) return size;

	for( size_t y = 0; y < table.size(); y++ )
		size.Y += table[y].front().paddedSize().Y;
	for( size_t x = 0; x < table.front().size(); x++ )
		size.X += table.front()[x].paddedSize().X;
	return size;
}

void Table::fitSize()
{
	if( empty() ) return;
	// Size: Normalized Size
	// ValueSize: Size of the Value
	// Padded Size: NormalizedSize + Padding

	std::vector<float> widths( numCols() );
	std::vector<float> heights( numRows() );

	auto getMaxSize = [&]( const Cell& cell, size_t row, size_t col ) {
		if( cell.paddedSize().X > widths[col] ) widths[col] = cell.paddedSize().X;
		if( cell.paddedSize().Y > heights[row] ) heights[row] = cell.paddedSize().Y;
	};

	auto setMaxSize = [&]( Cell& cell, size_t row, size_t col ) {
		cell.size.X = widths[col] - cell.paddingSize().X;
		cell.size.Y = heights[row] - cell.paddingSize().Y;
	};

	forEach( getMaxSize );
	forEach( setMaxSize );
}

void Table::forEach( std::function<void( const Cell&, size_t, size_t )> f ) const
{
	for( size_t y = 0; y < numRows(); y++ )
		for( size_t x = 0; x < numCols(); x++ )
			f( table[y][x], y, x );
}
void Table::forEach( std::function<void( Cell&, size_t, size_t )> f )
{
	for( size_t y = 0; y < numRows(); y++ )
		for( size_t x = 0; x < numCols(); x++ )
			f( table[y][x], y, x );
}
void Table::forEach( std::function<bool( const Cell&, size_t, size_t )> f ) const
{
	for( size_t y = 0; y < numRows(); y++ )
		for( size_t x = 0; x < numCols(); x++ )
			if( f( table[y][x], y, x ) ) return;
}
void Table::forEach( std::function<bool( Cell&, size_t, size_t )> f )
{
	for( size_t y = 0; y < numRows(); y++ )
		for( size_t x = 0; x < numCols(); x++ )
			if( f( table[y][x], y, x ) ) return;
}

void Table::realign()
{
	if( empty() ) return;
	auto getAlignment = []( float diff, Table::Align align ) {
		switch( align ) {
			case Table::Align::Top:
			case Table::Align::Left: return 0.0f;
			default:
			case Table::Align::Center: return diff / 2.0f;
			case Table::Align::Bottom:
			case Table::Align::Right: return diff;
		}
	};

	auto alignCell = [&]( Cell& cell, size_t row, size_t col ) {
		Vector2F diff {
			cell.size.X - cell.valueSize.X,
			cell.size.Y - cell.valueSize.Y
		};
		cell.valueOffset = Vector2F{
			getAlignment( diff.X, cell.hAlign ),
			getAlignment( diff.Y, cell.vAlign )
		};
	};

	forEach( alignCell );
}

Vector2F Table::getCellOrigin( size_t row, size_t col ) const
{
	Vector2F cellOrigin{ 0.0f, 0.0f };
	for( size_t y = 0; y < numRows(); y++ )
	{
		if( y == row ) break;
		cellOrigin.Y += table[y].front().paddedSize().Y;
	}

	for( size_t x = 0; x < numCols(); x++ )
	{
		if( x == col ) break;
		cellOrigin.X += table.front()[x].paddedSize().X;
	}

	return cellOrigin;
}
