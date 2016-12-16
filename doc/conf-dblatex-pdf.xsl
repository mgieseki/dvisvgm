<?xml version='1.0' encoding="iso-8859-1"?>
<!-- This file is part of dvisvgm -->
<!-- Copyright (C) 2015-2016 Martin Gieseking <martin.gieseking@uos.de> -->
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:exsl="http://exslt.org/common"
	xmlns:func="http://exslt.org/functions"
	xmlns:my="my-namespace"
	extension-element-prefixes="exsl func">
	
	<xsl:param name="xetex.font">
		<xsl:text>\setmainfont{Source Sans Pro}&#10;</xsl:text>
		<xsl:text>\setsansfont{Source Sans Pro}&#10;</xsl:text>
		<xsl:text>\setmonofont{Source Code Pro}&#10;</xsl:text>
	</xsl:param>

	<xsl:param name="doc.toc.show">0</xsl:param>
	<xsl:param name="refentry.numbered">0</xsl:param>
	<xsl:param name="figure.caution">caution</xsl:param>
	<xsl:param name="figure.note">note</xsl:param>
	<xsl:param name="table.default.tabstyle">tabular</xsl:param>
	<xsl:param name="table.in.float">0</xsl:param>
	<xsl:param name="term.breakline">1</xsl:param>

	<xsl:param name="literal.layout.options">
		<xsl:text>basicstyle=\ttfamily\small,backgroundcolor=\color[gray]{0.9},columns=fullflexible,frame=single</xsl:text>
	</xsl:param>

	<!-- ensure processing of refmiscinfo elements -->
	<xsl:template match="refentry">
		<xsl:apply-templates select="refmeta/refmiscinfo"/>
		<xsl:apply-imports/>
	</xsl:template>

	<!-- put refmiscinfo data into the TeX file -->
	<xsl:template match="refmiscinfo[@class]">
		<xsl:value-of select="concat('\def\refmiscinfo', @class, '{', ., '}&#10;')"/>
	</xsl:template>

	<!-- output TOC before synopsis section -->
	<xsl:template match="refsynopsisdiv">
		<xsl:text>\tableofcontents&#10;</xsl:text>
		<xsl:call-template name="makeheading">
			<xsl:with-param name="level">2</xsl:with-param>
			<xsl:with-param name="num">1</xsl:with-param>
		</xsl:call-template>
		<xsl:apply-templates/>
	</xsl:template>

	<!-- force non-star \subsection commands  -->
	<xsl:template match="refsect1">
		<xsl:call-template name="makeheading">
			<xsl:with-param name="level">2</xsl:with-param>
			<xsl:with-param name="num">1</xsl:with-param>
		</xsl:call-template>
		<xsl:apply-templates/>
	</xsl:template>
	
	<xsl:template match="refsect1/title/text()">
		<xsl:param name="name"/>
		<xsl:variable name="sections">
			<section>Description</section>
			<section>Options</section>
			<section>Supported Specials</section>
			<section>Examples</section>
			<section>Environment</section>
			<section>Files</section>
			<section>See also</section>
			<section>Resources</section>
			<section>Bugs</section>
			<section>Author</section>
			<section>Copying</section>
		</xsl:variable>
		<xsl:variable name="mixed-name" select="exsl:node-set($sections)/section[my:toupper(.)=current()]"/>
		<xsl:choose>
			<xsl:when test="$mixed-name">
				<xsl:value-of select="$mixed-name"/>
			</xsl:when>
			<xsl:otherwise>
				<value-of select="."/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<func:function name="my:toupper">
		<xsl:param name="str"/>
		<func:result select="translate($str, 'abcdefghijklmnopqrstuvwxyz', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
	</func:function>
</xsl:stylesheet>
