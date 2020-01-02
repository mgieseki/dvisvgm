<?xml version="1.0"?>
<!-- This file is part of dvisvgm -->
<!-- Copyright (C) 2015-2020 Martin Gieseking <martin.gieseking@uos.de> -->
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:my="my-namespace"
	exclude-result-prefixes="my">

	<xsl:import href="tweak-db-refentry.xsl"/>
	<xsl:output doctype-public="-//OASIS//DTD DocBook XML V4.5//EN" doctype-system="http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd"/>

	<!-- add subtitle -->
	<xsl:template match="articleinfo">
		<xsl:copy>
			<title>dvisvgm {VERSION} Manual</title>
			<subtitle>A fast DVI to SVG converter</subtitle>
			<xsl:apply-templates select="*[not(self::title)]"/>
		</xsl:copy>
	</xsl:template>

	<!-- adapt ISO date created by keyword expansion -->
	<xsl:template match="articleinfo/date | revision[1]/date">
		<date>
			<xsl:value-of select="my:extract-date(.)"/>
		</date>
	</xsl:template>

	<!-- drop redundant section 'Name' -->
	<xsl:template match="section[@id='_name']"/>
</xsl:stylesheet>
